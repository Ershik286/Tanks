#include <iostream>
#include <Windows.h>
#include <chrono>
#include <thread>
#include <string>
#include <gdiplus.h>
#include <random>
#include <tchar.h>
#include <ctime>
#include <fstream>
#include <direct.h>
#include <vector>
#include <lmcons.h>
#include <ShlObj.h>
#include <locale>
#include <codecvt>
#include <sstream>
#include "Block.h"
#include "Tank.h"
#include "Bullet.h"

#define TIMER_ONE_PLAYER_ID 0
#define TIMER_TWO_PLAYER_ID 1
#define TIMER_THREE_PLAYER_ID 2
#define TIMER_FOUR_PLAYER_ID 3

#define TIMER_INTERVAL 30
#define TIMER_ON
#define M_PI 3.14

#define IDC_COMBOBOX_PLAYERS 10
#define IDC_BUTTON_START_GAME 11

using namespace std;

const int WIDTHFIELD = 495;
const int HEIGHTFIELD = 551;

const int WIDTHBLOCK = 30;
const int HEIGHTBLOCK = 32;
const int demension = 16;
const int shift = 0;

//30 / 32 bloks, bloks - 16/16 pixel

Block Map1[HEIGHTBLOCK][WIDTHBLOCK];
Block Map2[HEIGHTBLOCK][WIDTHBLOCK]; //Unused, commented
Block Map3[HEIGHTBLOCK][WIDTHBLOCK]; //Unused, commented

enum class GameState {
    PLAYING,
    PLAYER_1_WINS,
    PLAYER_2_WINS,
    PLAYER_3_WINS,
    PLAYER_4_WINS,
    GAME_OVER,
    DRAW
};

GameState gameState = GameState::PLAYING; // Изначально игра идет

HDC hdc;
int level = 0;
int playCount = 2; //количество танков
int chanceHealth = 1;//1% шанса спавна коробки

int frameCount = 0;
auto startTime = std::chrono::high_resolution_clock::now();
double fps = 0.0; // Переменная для хранения FPS

HBRUSH greenBrush = CreateSolidBrush(RGB(0, 255, 0)); // Зеленый танк
HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
HBRUSH leavesBrush = CreateSolidBrush(RGB(50, 135, 50));
HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
HBRUSH fieldBrush = CreateSolidBrush(RGB(50, 50, 50));
HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));

HPEN tankPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0)); // Чёрная обводка

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"Tanks";

    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    // Create Window
    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Tanks",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        //size Window
        CW_USEDEFAULT, CW_USEDEFAULT, WIDTHFIELD + 125, HEIGHTFIELD,

        NULL,       // Parent window
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

int timeMoving = 0;

/*Function*/

void drawUnit(HWND hwnd, Tank& player, HBRUSH brush);
RECT GetUnitRect(Tank& player);
void createOneLevel();
void Draw(HDC hdc, const RECT& drawArea, HWND hwnd);
//RECT oldUnitRect; //Unused
void ClearBackGround(HDC hdc, HWND hwnd, RECT rect);
void createTank(Tank& player, int x1, int y1, int x2, int y2);
void DrawHealthBar(HDC hdc, int x, int y, int health, int maxHealth, HBRUSH barBrush);
void createMenu(HWND hwnd);

vector<Tank> players;
vector<Bullet> depthBullet{};

RECT messageRect = { 50, 35, 250, 50 }; // Область вывода сообщения о победе

int xMenuStart = 495; //505 + 70 = 575 - max
int yHealthBarStart = 200;
int yMenuStart = 10; //20 * playCount
int xMenuEnd = 90;

/*Function*/

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static RECT menuRect = { xMenuStart, yMenuStart, xMenuStart + 70, yMenuStart + 20 * playCount + 30 * (playCount - 1) + yHealthBarStart };
    static int sX, sY;
    //static RECT updateRect = { 0, 0, 0, 0 }; //Removed unused variable
    static HWND hComboPlayers, hComboLabs, hComboControlWorks; // Store edit control handles
    switch (uMsg) {
    case WM_SIZE:
        sX = LOWORD(lParam);
        sY = LOWORD(wParam);
        break; // Add break here

    case WM_CREATE:
        createMenu(hwnd);
        players.resize(playCount);
        createTank(players[0], 16 * 16 + shift, 28 * 16 + shift, 16 * 17 + shift, 29 * 16 + shift); // Зеленый танк внизу
        createTank(players[1], 16 * 16 + shift, 2 * 16 + shift, 17 * 16 + shift, 3 * 16 + shift);  // Красный танк вверху
        createOneLevel();
        for (int i = 0; i < playCount; i++) {
            SetTimer(hwnd, TIMER_ONE_PLAYER_ID + i, TIMER_INTERVAL, NULL);
        }

        break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        hdc = BeginPaint(hwnd, &ps);
        SetBkMode(hdc, OPAQUE); //ADDED CODE.

        // Сначала рисуем карту - ONLY inside ps.rcPaint!
        Draw(hdc, ps.rcPaint, hwnd);

        // Затем рисуем танки
        drawUnit(hwnd, players[0], greenBrush);
        drawUnit(hwnd, players[1], redBrush);

        // Draw health bars for each player
        for (int i = 0; i < playCount; i++) {
            HBRUSH healthBarBrush;
            switch (i) {
            case 0:
                healthBarBrush = CreateSolidBrush(RGB(0, 255, 0));
                break;
            case 1:
                healthBarBrush = CreateSolidBrush(RGB(255, 0, 0));
                break;
            case 2:
                healthBarBrush = CreateSolidBrush(RGB(255, 255, 0));
                break;
            case 3:
                healthBarBrush = CreateSolidBrush(RGB(0, 0, 255));
                break;
            default:
                healthBarBrush = CreateSolidBrush(RGB(128, 128, 128));
                break;
            }

            // Calculate HP
            int health = players[i].GetHealth();
            int maxHealth = 100;  // Assuming max health is always 100

            // Calculate the position of each healthBar
            int x = xMenuStart;
            int y = yMenuStart + (i * 30) + yHealthBarStart;

            // Draw
            DrawHealthBar(hdc, x, y, health, maxHealth, healthBarBrush);

            DeleteObject(healthBarBrush);
        }

        for (auto& bullet : depthBullet) {
            bullet.Draw(hdc);
        }

        depthBullet.clear();

        for (int i = 0; i < playCount; i++) {

            // Рисуем пули первого игрока
            for (auto& bullet : players[i].GetBullets()) {
                bullet.Draw(hdc);
            }
        }
        //  Отображение сообщения о победе
        if (gameState != GameState::PLAYING) {
            wstring message;
            if (gameState == GameState::PLAYER_1_WINS) {
                message = L"Победил первый игрок!";
            }
            else if (gameState == GameState::PLAYER_2_WINS) {
                message = L"Победил второй игрок!";
            }
            else if (gameState == GameState::PLAYER_3_WINS) {
                message = L"Победил третий игрок!";
            }
            else if (gameState == GameState::PLAYER_4_WINS) {
                message = L"Победил третий игрок!";
            }
            else if (gameState == GameState::GAME_OVER) {
                message = L"Увы, вы проиграли!";
            }
            else {
                message = L"Ничья!";
            }

            FillRect(hdc, &messageRect, whiteBrush); // Закрашиваем белым

            SetTextColor(hdc, RGB(0, 0, 0)); // Черный текст
            SetBkMode(hdc, OPAQUE);
            DrawText(hdc, message.c_str(), -1, &messageRect, DT_CENTER | DT_VCENTER | DT_WORDBREAK);
        }

        //frameCount++;

        //auto currentTime = std::chrono::high_resolution_clock::now();
        //auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();

        //if (elapsedTime >= 1) { // Обновляем FPS каждые 1 секунду
        //    fps = (float)frameCount / elapsedTime; // Теперь fps - double
        //    frameCount = 0;
        //    startTime = currentTime;
        //}
        //// Вывод FPS в окно
        //wchar_t fpsText[32];
        //swprintf_s(fpsText, 32, L"FPS: %.2f", fps);  // Форматируем FPS в строку
        //TextOutW(hdc, 17, 20, fpsText, (int)wcslen(fpsText)); // Выводим текст в верхнем левом углу

        EndPaint(hwnd, &ps);
        break;
    }
    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        if (wmId == IDC_BUTTON_START_GAME) {
            hComboPlayers = GetDlgItem(hwnd, IDC_COMBOBOX_PLAYERS);
            playCount = SendMessageW(hComboPlayers, CB_GETCURSEL, 0, 0); // Get selected item index
        }
    }

    case WM_LBUTTONDOWN: {
        int xPos = LOWORD(lParam);
        int yPos = HIWORD(lParam);
        break; // Add Break here
    }

    case WM_RBUTTONDOWN: {
        int xPos = LOWORD(lParam);
        int yPos = HIWORD(lParam);
        break;
    }
    case WM_TIMER: {
        RECT updateRect = { 0, 0, 0, 0 };
        int numberTankDeath = 0;
        int numTanksAlive = 0;
        int winningPlayerIndex = -1;

        for (int playerIndex = 0; playerIndex < playCount; ++playerIndex) {
            if (players[playerIndex].isAlive()) {
                numTanksAlive++;
                winningPlayerIndex = playerIndex;
            }
            else continue;
            if (wParam != TIMER_ONE_PLAYER_ID + playerIndex) continue;

            RECT oldTankRect = GetUnitRect(players[playerIndex]);
            players[playerIndex].movement(Map1, players, hwnd);
            RECT newTankRect = GetUnitRect(players[playerIndex]);

            UnionRect(&updateRect, &updateRect, &oldTankRect);
            UnionRect(&updateRect, &updateRect, &newTankRect);
            // Check for bullets
            for (size_t bulletIndex = 0; bulletIndex < players[playerIndex].GetBullets().size(); ) {
                Bullet& bullet = players[playerIndex].GetBullets()[bulletIndex];
                RECT oldBulletRect = bullet.GetRect();

                bullet.Move(Map1, hwnd, players, playerIndex);

                if (!bullet.IsAlive()){
                    // Bullet is dead, add the area to redraw
                    UnionRect(&updateRect, &updateRect, &oldBulletRect);
                    depthBullet.push_back(move(bullet));
                    players[playerIndex].GetBullets().erase(players[playerIndex].GetBullets().begin() + bulletIndex);
                }
                else {
                    // Bullet is still alive
                    RECT newBulletRect = bullet.GetRect();
                    UnionRect(&updateRect, &updateRect, &oldBulletRect);
                    UnionRect(&updateRect, &updateRect, &newBulletRect);
                    ++bulletIndex;
                }
            }
        }
        if (numTanksAlive == 0) {
            gameState = GameState::DRAW; // Tie
            InvalidateRect(hwnd, &messageRect, FALSE);
        }
        else if (numTanksAlive == 1) {

            switch (winningPlayerIndex) {
            case 0:
                gameState = GameState::PLAYER_1_WINS;
                break;
            case 1:
                if (playCount == 1) {
                    gameState = GameState::GAME_OVER;
                }
                else {
                    gameState = GameState::PLAYER_2_WINS;
                }
                break;
            case 2:
                gameState = GameState::PLAYER_3_WINS;
                break;
            case 3:
                gameState = GameState::PLAYER_4_WINS;
                break;
            default:
                // Should not reach here
                break;
            }
            InvalidateRect(hwnd, &messageRect, FALSE);
        }
        else {
            gameState = GameState::PLAYING;
        }
        for (int i = 0; i < playCount; i++) {
            if (players[i].GetTempHealth() != players[i].GetHealth()) {
                InvalidateRect(hwnd, &menuRect, FALSE);
                players[i].SetHealth(players[i].GetHealth());
                break;
            }
        }
        InvalidateRect(hwnd, &updateRect, FALSE);
        break;
    }
    case WM_KEYDOWN: {
        int* tanksMoving{ new int[4] {0, 0, 0, 0} };
        if (gameState != GameState::PLAYING) break;
        switch (wParam) {
        case 'W':
            for (int i = 0; i < 4; i++) tanksMoving[i] = 0;
            tanksMoving[1] = 1;//Up
            players[0].setVectorMoving(tanksMoving);
            players[0].SetAngle(90); // Угол для движения вверх
            break;
        case 'A':
            for (int i = 0; i < 4; i++) tanksMoving[i] = 0;
            tanksMoving[2] = 1;//Left
            players[0].setVectorMoving(tanksMoving);
            players[0].SetAngle(180); // Угол для движения влево
            break;
        case 'S':
            for (int i = 0; i < 4; i++) tanksMoving[i] = 0;
            tanksMoving[3] = 1;//Down
            players[0].setVectorMoving(tanksMoving);
            players[0].SetAngle(270); // Угол для движения вниз
            break;
        case 'D':
            for (int i = 0; i < 4; i++) tanksMoving[i] = 0;
            tanksMoving[0] = 1;//Right
            players[0].setVectorMoving(tanksMoving);
            players[0].SetAngle(0); // Угол для движения вправо
            break;
        case VK_UP:
            for (int i = 0; i < 4; i++) tanksMoving[i] = 0;
            tanksMoving[1] = 1;
            players[1].setVectorMoving(tanksMoving);
            players[1].SetAngle(90);
            break;
        case VK_DOWN:
            for (int i = 0; i < 4; i++) tanksMoving[i] = 0;
            tanksMoving[3] = 1;
            players[1].setVectorMoving(tanksMoving);
            players[1].SetAngle(270);
            break;
        case VK_LEFT:
            for (int i = 0; i < 4; i++) tanksMoving[i] = 0;
            tanksMoving[2] = 1;
            players[1].setVectorMoving(tanksMoving);
            players[1].SetAngle(180);
            break;
        case VK_RIGHT:
            for (int i = 0; i < 4; i++) tanksMoving[i] = 0;
            tanksMoving[0] = 1;
            players[1].setVectorMoving(tanksMoving);
            players[1].SetAngle(0);
            break;
        case VK_SPACE: // Стрельба для первого игрока
            players[0].Shoot(Map1);
            break;
        case VK_SHIFT: // Стрельба для второго игрока
            players[1].Shoot(Map1);
            break;
        default:
            break;
        }
        break;
        delete[] tanksMoving;
    }
    case WM_KEYUP: {
        switch (wParam)
        {
        case 'W':
            players[0].movingVector[1] = 0;
            break;
        case 'A':
            players[0].movingVector[2] = 0;
            break;
        case 'S':
            players[0].movingVector[3] = 0;
            break;
        case 'D':
            players[0].movingVector[0] = 0;
            break;
        case VK_UP:
            players[1].movingVector[1] = 0;
            break;
        case VK_DOWN:
            players[1].movingVector[3] = 0;
            break;

        case VK_LEFT:
            players[1].movingVector[2] = 0;
            break;
        case VK_RIGHT:
            players[1].movingVector[0] = 0;
            break;

        default:
            break;
        }
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void createTank(Tank& player, int x1, int y1, int x2, int y2) {
    player.position.x1 = x1;
    player.position.y1 = y1;
    player.position.x2 = x2;
    player.position.y2 = y2;
    player.tempPosition = player.position; // Важно инициализировать tempPosition
}

RECT GetUnitRect(Tank& player) {
    RECT unitRect;
    unitRect.left = player.position.x1;
    unitRect.top = player.position.y1;
    unitRect.right = player.position.x2 + shift;
    unitRect.bottom = player.position.y2 + shift;
    return unitRect;
}

void ClearBackGround(HDC hdc, HWND hwnd, RECT rect) {
    HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(hdc, &messageRect, brush);
    DeleteObject(brush);
}

void createOneLevel() {
    wchar_t appDataPath[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, appDataPath);
    wstring appDataDir(appDataPath);
    wstring wayDirectory = appDataDir + L"\\Tanks";
    wstring mapFilePath = wayDirectory + L"\\maps.txt";

    // Проверяем, существует ли файл с картами
    ifstream checkFile(mapFilePath);
    if (!checkFile.good()) {
        // Первый запуск - создаем директорию и файл
        CreateDirectoryW(wayDirectory.c_str(), NULL);

        for (int i = 0; i < HEIGHTBLOCK; i++) {
            for (int j = 0; j < WIDTHBLOCK; j++) {
                int type = Block::emptiness;
                if (i == 0 || i == HEIGHTBLOCK - 1) {
                    type = Block::border;
                }
                if (j == 0 || j == WIDTHBLOCK - 1) {
                    type = Block::border;
                }

                int randomHealth = rand() % (100 - 0 + 1) + 0; //шанс 1%
                if (randomHealth < chanceHealth) { //1%
                    type = Block::healthBox;
                }

                Map1[i][j].initialization(type);
                Map1[i][j].positionBlock(i, j);
            }
        }
        for (int i = 4; i < 8; i++) {
            for (int j = 3; j < 5; j++) {
                Map1[i][j].initialization(Block::brick);
                Map1[i][j].positionBlock(i, j);
            }
            for (int j = 7; j < 9; j++) {
                Map1[i][j].initialization(Block::brick);
                Map1[i][j].positionBlock(i, j);
            }
            for (int j = 11; j < 13; j++) {
                Map1[i][j].initialization(Block::brick);
                Map1[i][j].positionBlock(i, j);
            }
            for (int j = 15; j < 16; j++) {
                Map1[i][j].initialization(Block::brick);
                Map1[i][j].positionBlock(i, j);
            }
            for (int j = 18; j < 19; j++) {
                Map1[i][j].initialization(Block::brick);
                Map1[i][j].positionBlock(i, j);
            }
        }

        for (int i = 4; i < 6; i++) {
            for (int j = 21; j < WIDTHBLOCK - 3; j++) {
                Map1[i][j].initialization(Block::brick);
                Map1[i][j].positionBlock(i, j);
            }
        }

        Map1[5][16].initialization(Block::brick);
        Map1[5][16].positionBlock(5, 16);

        Map1[5][17].initialization(Block::brick);
        Map1[5][17].positionBlock(5, 17);

        //Map1[HEIGHTBLOCK - 2][14].initialization(Block::base);

        int blockType = Block::emptiness;

        if (blockType == Block::emptiness) {
            int randomHealth = rand() % (100 - 0 + 1) + 0; //шанс 8%
            if (randomHealth < 1) {
                blockType = Block::healthBox;
            }
        }

        Map1[HEIGHTBLOCK - 2][14].initialization(blockType);
        Map1[HEIGHTBLOCK - 2][14].positionBlock(HEIGHTBLOCK - 2, 14);

        for (int i = 10; i < 12; i++) {
            for (int j = 2; j < 8; j++) {
                Map1[i][j].initialization(Block::brick);
                Map1[i][j].positionBlock(i, j);
            }

            for (int j = 10; j < 16; j++) {
                Map1[i][j].initialization(Block::brick);
                Map1[i][j].positionBlock(i, j);
            }
            for (int j = 18; j < 19; j++) {
                Map1[i][j].initialization(Block::brick);
                Map1[i][j].positionBlock(i, j);
            }
            for (int j = 21; j < WIDTHBLOCK - 3; j++) {
                Map1[i][j].initialization(Block::brick);
                Map1[i][j].positionBlock(i, j);
            }
        }

        for (int i = 14; i < 17; i++) {
            for (int j = 2; j < 4; j++) {
                Map1[i][j].initialization(Block::brick);
                Map1[i][j].positionBlock(i, j);
            }
            for (int j = 6; j < 9; j++) {
                Map1[i][j].initialization(Block::brick);
                Map1[i][j].positionBlock(i, j);
            }
            for (int j = 12; j < 16; j++) {
                Map1[i][j].initialization(Block::brick);
                Map1[i][j].positionBlock(i, j);
            }
            for (int j = 18; j < 19; j++) {
                Map1[i][j].initialization(Block::brick);
                Map1[i][j].positionBlock(i, j);
            }
            for (int j = 21; j < WIDTHBLOCK - 3; j++) {
                Map1[i][j].initialization(Block::brick);
                Map1[i][j].positionBlock(i, j);
            }
        }

        for (int i = 22; i < 26; i++) {
            for (int j = 2; j < 4; j++) {
                Map1[i][j].initialization(Block::brick);
                Map1[i][j].positionBlock(i, j);
            }
            for (int j = 6; j < 9; j++) {
                Map1[i][j].initialization(Block::brick);
                Map1[i][j].positionBlock(i, j);
            }
            for (int j = 12; j < 16; j++) {
                Map1[i][j].initialization(Block::brick);
                Map1[i][j].positionBlock(i, j);
            }
            for (int j = 18; j < 19; j++) {
                Map1[i][j].initialization(Block::brick);
                Map1[i][j].positionBlock(i, j);
            }
            for (int j = 21; j < WIDTHBLOCK - 3; j++) {
                Map1[i][j].initialization(Block::brick);
                Map1[i][j].positionBlock(i, j);
            }
        }

        // Сохраняем карту в файл
        ofstream fileMap(mapFilePath, ios::out);
        if (fileMap.is_open()) {
            for (int i = 0; i < HEIGHTBLOCK; i++) {
                for (int j = 0; j < WIDTHBLOCK; j++) {
                    fileMap << Map1[i][j].getNumberBlock();
                }
                fileMap << endl;
            }
            fileMap.close();
        }
    }
    else {
        ifstream fileMap(mapFilePath, ios::in);
        if (fileMap.is_open()) {
            string line;
            int i = 0;
            while (getline(fileMap, line) && i < HEIGHTBLOCK) {
                for (int j = 0; j < line.length() && j < WIDTHBLOCK; j++) {
                    int blockType = line[j] - '0'; // Конвертируем символ в число
                    if (blockType == Block::emptiness) {
                        int randomHealth = rand() % (100 - 0 + 1) + 0; //шанс 1%
                        if (randomHealth < 1) { //1%
                            blockType = Block::healthBox;
                        }
                    }
                    Map1[i][j].initialization(blockType);
                    Map1[i][j].positionBlock(i, j);
                }
                i++;
            }
            fileMap.close();
        }
    }
}

void createTwoLevel() {

}

void createThreeLevel() {

}

void drawBlock(RECT& blockRect, Block block);

void Draw(HDC hdc, const RECT& drawArea, HWND hwnd) {
    // Convert RECT values to a string

    // Create brushes and pens (Moved here for efficiency if they aren't changing)
    HBRUSH borderBrush = CreateSolidBrush(RGB(128, 128, 128));
    HBRUSH brickBrush = CreateSolidBrush(RGB(128, 15, 0));
    HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
    HBRUSH fieldBrush = CreateSolidBrush(RGB(50, 50, 50)); // Define the field brush here
    HPEN blackPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

    // Set the background mode to OPAQUE for proper filling (important!)
    SetBkMode(hdc, OPAQUE);

    // Save old objects (store for restore, even with double buffering)
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, fieldBrush); //Use fieldBrush as the background
    HPEN oldPen = (HPEN)SelectObject(hdc, blackPen);
    // Moved all of this outside your for loop, you aren't needing them
    stringstream ss;
    string message;
    wstring wideMessage;
    for (int i = 0; i < HEIGHTBLOCK; ++i) {
        for (int j = 0; j < WIDTHBLOCK; ++j) {
            RECT block = {
                Map1[i][j].Position.x1,
                Map1[i][j].Position.y1,
                Map1[i][j].Position.x2,
                Map1[i][j].Position.y2
            };
            RECT intersection;
            if (IntersectRect(&intersection, &drawArea, &block)) {
                int blockType = Map1[i][j].getNumberBlock();

                switch (blockType) {
                case Block::emptiness:
                    FillRect(hdc, &intersection, fieldBrush); // Always fill the intersected area
                    //FrameRect(hdc, &intersection, blackBrush);
                    break;
                case Block::border: {
                    //Need to create the string stream inside the function, but at the border
                    //ss.str("");
                    //ss.clear();
                    //ss << "Old Rect: Left=" << drawArea.left << ", Top=" << drawArea.top << ", Right=" << drawArea.right << ", Bottom=" << drawArea.bottom;
                    //message = ss.str();

                    //// Correct wstring conversion
                    //wideMessage = wstring(message.begin(), message.end());

                    //MessageBoxW(hwnd, wideMessage.c_str(), L"Old Tank Rect", MB_OK);
                    FillRect(hdc, &intersection, borderBrush);
                    FrameRect(hdc, &intersection, blackBrush);
                    break;
                }
                case Block::brick: {
                    RECT blockFract;
                    blockFract.left = Map1[i][j].Position.x1;
                    blockFract.right = Map1[i][j].Position.x2 - 8;
                    blockFract.top = Map1[i][j].Position.y1;
                    blockFract.bottom = Map1[i][j].Position.y2 - 8;

                    if (IntersectRect(&intersection, &drawArea, &blockFract)) {
                        FillRect(hdc, &blockFract, brickBrush); // Use blockFract for fill
                        FrameRect(hdc, &blockFract, blackBrush); // Use blockFract for frame
                    }

                    blockFract.left = Map1[i][j].Position.x1;
                    blockFract.right = Map1[i][j].Position.x2 - 8;
                    blockFract.top = Map1[i][j].Position.y1 + 8;
                    blockFract.bottom = Map1[i][j].Position.y2;
                    if (IntersectRect(&intersection, &drawArea, &blockFract)) {
                        FillRect(hdc, &blockFract, brickBrush); // Use blockFract for fill
                        FrameRect(hdc, &blockFract, blackBrush); // Use blockFract for frame
                    }

                    blockFract.left = Map1[i][j].Position.x1 + 8;
                    blockFract.right = Map1[i][j].Position.x2;
                    blockFract.top = Map1[i][j].Position.y1 + 8;
                    blockFract.bottom = Map1[i][j].Position.y2;
                    if (IntersectRect(&intersection, &drawArea, &blockFract)) {
                        FillRect(hdc, &blockFract, brickBrush); // Use blockFract for fill
                        FrameRect(hdc, &blockFract, blackBrush); // Use blockFract for frame
                    }

                    blockFract.left = Map1[i][j].Position.x1 + 8;
                    blockFract.right = Map1[i][j].Position.x2;
                    blockFract.top = Map1[i][j].Position.y1;
                    blockFract.bottom = Map1[i][j].Position.y2 - 8;
                    if (IntersectRect(&intersection, &drawArea, &blockFract)) {
                        FillRect(hdc, &blockFract, brickBrush); // Use blockFract for fill
                        FrameRect(hdc, &blockFract, blackBrush); // Use blockFract for frame
                    }

                    break;
                }
                case Block::foliage:
                    FillRect(hdc, &intersection, leavesBrush);
                    break;

                case Block::healthBox:
                    FillRect(hdc, &intersection, whiteBrush);
                    int centerX = intersection.left + (intersection.right - intersection.left) / 2;
                    int centerY = intersection.top + (intersection.bottom - intersection.top) / 2;

                    HPEN redPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
                    HPEN oldRedPen = (HPEN)SelectObject(hdc, redPen);

                    // Вертикальная линия
                    MoveToEx(hdc, centerX, intersection.top, NULL);
                    LineTo(hdc, centerX, intersection.bottom);

                    // Горизонтальная линия
                    MoveToEx(hdc, intersection.left, centerY, NULL);
                    LineTo(hdc, intersection.right, centerY);

                    SelectObject(hdc, oldRedPen);
                    DeleteObject(redPen);
                    FrameRect(hdc, &intersection, blackBrush);
                    break;
                }
            }
        }
    }

    // Restore context
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);

    // Delete created objects (if created in the function!)
    DeleteObject(borderBrush);
    DeleteObject(brickBrush);
    DeleteObject(whiteBrush);
    DeleteObject(blackPen);
}

void drawUnit(HWND hwnd, Tank& player, HBRUSH brush) {
    if (!player.isAlive()) return;
    HDC hdc = GetDC(hwnd);
    HBRUSH blackBrush = CreateSolidBrush(RGB(60, 60, 60));

    if (player.position.x1 != player.tempPosition.x1 || player.position.y1 != player.tempPosition.y1) {
        // Очищаем предыдущую позицию
        RECT oldRect = {
            player.tempPosition.x1,
            player.tempPosition.y1,
            player.tempPosition.x2,
            player.tempPosition.y2
        };
        int blockX1 = player.tempPosition.x1 / 16;  // Делим на размер блока (16)
        int blockY1 = player.tempPosition.y1 / 16;
        int blockX2 = player.tempPosition.x2 / 16;
        int blockY2 = player.tempPosition.y2 / 16;

        // Перерисовываем блоки под танком, if they are in the area.
        for (int y = blockY1; y <= blockY2; ++y) {
            for (int x = blockX1; x <= blockX2; ++x) {
                if (x >= 0 && x < WIDTHBLOCK && y >= 0 && y < HEIGHTBLOCK) { // Проверка границ
                    RECT blockRect;
                    blockRect.left = x * 16;
                    blockRect.top = y * 16;
                    blockRect.right = blockRect.left + 16;
                    blockRect.bottom = blockRect.top + 16;
                    drawBlock(blockRect, Map1[y][x]);
                }
            }
        }
    }

    // Рисуем танк
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
    HPEN oldPen = (HPEN)SelectObject(hdc, tankPen);

    if (!player.isAlive()) {
        SelectObject(hdc, blackBrush);
    }

    // Корпус танка
    Rectangle(hdc,
        player.position.x1,
        player.position.y1,
        player.position.x2,
        player.position.y2);

    // Внутренний прямоугольник
    Rectangle(hdc,
        player.position.x1 + 4,
        player.position.y1 + 4,
        player.position.x2 - 4,
        player.position.y2 - 4);

    // Рисуем пушку
    int centerX = (player.position.x1 + player.position.x2) / 2;
    int centerY = (player.position.y1 + player.position.y2) / 2;
    double angleRad = player.GetAngle() * M_PI / 180.0;
    int gunLength = 7;
    int gunEndX = centerX + static_cast<int>(gunLength * cos(angleRad));
    int gunEndY = centerY - static_cast<int>(gunLength * sin(angleRad));

    HPEN gunPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
    HPEN oldGunPen = (HPEN)SelectObject(hdc, gunPen);

    MoveToEx(hdc, centerX, centerY, NULL);
    LineTo(hdc, gunEndX, gunEndY);

    // Восстанавливаем ресурсы
    SelectObject(hdc, oldGunPen);
    DeleteObject(gunPen);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);

    player.tempPosition = player.position;
    ReleaseDC(hwnd, hdc);
    DeleteObject(blackBrush);  // Clean up the blackBrush!
}

void drawBlock(RECT& blockRect, Block block) {
    if (block.getNumberBlock() == Block::foliage) {
        FillRect(hdc, &blockRect, leavesBrush);
    }
    if (block.getNumberBlock() == Block::emptiness) {
        FillRect(hdc, &blockRect, fieldBrush);
    }
    if (block.getNumberBlock() == Block::healthBox) {
        // Корректируем координаты, но не нарушаем логику
        int padding = 2;
        RECT innerRect = {
            blockRect.left + padding,
            blockRect.top + padding,
            blockRect.right - padding,
            blockRect.bottom - padding
        };

        FillRect(hdc, &innerRect, fieldBrush);
        int centerX = (innerRect.left + innerRect.right) / 2;
        int centerY = (innerRect.top + innerRect.bottom) / 2;

        HPEN crossPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
        HPEN oldcrossPen = (HPEN)SelectObject(hdc, crossPen);

        MoveToEx(hdc, centerX, innerRect.top, NULL);
        LineTo(hdc, centerX, innerRect.bottom);
        MoveToEx(hdc, innerRect.left, centerY, NULL);
        LineTo(hdc, innerRect.right, centerY);

        FrameRect(hdc, &innerRect, blackBrush);

        SelectObject(hdc, oldcrossPen);
        DeleteObject(crossPen);
    }
    if (block.getNumberBlock() == Block::damageUpBox) {

    }
    if (block.getNumberBlock() == Block::speedUpBox) {

    }
}

void DrawHealthBar(HDC hdc, int x, int y, int health, int maxHealth, HBRUSH barBrush) {
    int healthBarWidth = (int)(((double)health / (double)maxHealth) * 70);

    RECT healthBarRect = { x, y, x + 70, y + 20 };
    RECT currentHealthRect = { x, y, x + healthBarWidth, y + 20 };

    HBRUSH backgroundHPBrush = CreateSolidBrush(RGB(255, 255, 255));
    FillRect(hdc, &healthBarRect, backgroundHPBrush);

    FillRect(hdc, &currentHealthRect, barBrush);
    FrameRect(hdc, &healthBarRect, CreateSolidBrush(RGB(0, 0, 0)));

    DeleteObject(backgroundHPBrush);
}

bool isColliding(Tank::Position pos1, Block::position pos2) {
    return !(pos1.x2 < pos2.x1 || pos1.x1 > pos2.x2 || pos1.y2 < pos2.y1 || pos1.y1 > pos2.y2);
}

void createMenu(HWND hwnd) {

    HWND comboPlayers = CreateWindowW(L"COMBOBOX", L"",
        CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE,
        xMenuStart, yMenuStart, xMenuEnd, yMenuStart + 80, hwnd, (HMENU)IDC_COMBOBOX_PLAYERS,
        GetModuleHandle(NULL), NULL);

    wstring maxCountPlayers[] = { L"1 Players", L"2 Players", L"3 Players", L"4 Players"};
    for (auto& discipline : maxCountPlayers) {
        SendMessageW(comboPlayers, CB_ADDSTRING, 0, (LPARAM)discipline.c_str());
    }
    SendMessageW(comboPlayers, CB_SETCURSEL, 0, 0); // Выбираем первый элемент по умолчанию

    CreateWindowW(L"BUTTON", L"Начать игру", WS_VISIBLE | WS_CHILD, xMenuStart, yMenuStart + 100, 90, 30, hwnd, (HMENU)IDC_BUTTON_START_GAME, GetModuleHandle(NULL), NULL);
}
