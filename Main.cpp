#include <iostream>
#include <Windows.h>
#include <chrono>
#include <thread>
#include <string>
#include <gdiplus.h> // Include for GDI+ (Handles JPEG and PNG)
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
#include "Block.h"
#include "Tank.h"
#include "Bullet.h"

#define TIMER_ONE_PLAYER_ID 1
#define TIMER_TWO_PLAYER_ID 2
#define TIMER_THREE_PLAYER_ID 3
#define TIMER_INTERVAL 30
#define TIMER_ON
#define M_PI 3.14

using namespace std;

const int WIDTH = 30;
const int HEIGHT = 32;
const int demension = 16;
const int shift = 0;

bool movingUp = false;
bool movingDown = false;
bool movingLeft = false;
bool movingRight = false;

//30 / 32 bloks, bloks - 16/16 pixel

Block Map1[HEIGHT][WIDTH];
Block Map2[HEIGHT][WIDTH];
Block Map3[HEIGHT][WIDTH];

HDC hdc;
int level = 0;
HBRUSH greenBrush = CreateSolidBrush(RGB(0, 255, 0)); // Зеленый танк
HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
HBRUSH fieldBrush = CreateSolidBrush(RGB(255, 255, 255));

HPEN tankPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0)); // Чёрная обводка

vector<Tank>Players;

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
        CW_USEDEFAULT, CW_USEDEFAULT, 495, 551,

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
void Draw();
RECT oldUnitRect;
void ClearBackGround(HDC hdc, HWND hwnd, RECT rect);
void createTank(Tank& player, int x1, int y1, int x2, int y2);

vector<Tank> players(2);
vector<Bullet> depthBullet{};

/*Function*/

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static int sX, sY;
    //static RECT updateRect = { 0, 0, 0, 0 };
    switch (uMsg) {
    case WM_SIZE:
        sX = LOWORD(lParam);
        sY = LOWORD(wParam);
        break; // Add break here

    case WM_CREATE:
        createTank(players[0], 16 * 16 + shift, 28 * 16 + shift, 16 * 17 + shift, 29 * 16 + shift); // Зеленый танк внизу
        createTank(players[1], 16 * 16 + shift, 2 * 16 + shift, 17 * 16 + shift, 3 * 16 + shift);  // Красный танк вверху
        createOneLevel();
        SetTimer(hwnd, TIMER_ONE_PLAYER_ID, TIMER_INTERVAL, NULL);
        SetTimer(hwnd, TIMER_TWO_PLAYER_ID, TIMER_INTERVAL, NULL);
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        hdc = BeginPaint(hwnd, &ps);

        // Сначала рисуем карту
        Draw();

        // Затем рисуем танки
        drawUnit(hwnd, players[0], greenBrush);
        drawUnit(hwnd, players[1], redBrush);

        for (auto& bullet : depthBullet) {
            bullet.Draw(hdc);
        }

        depthBullet.clear();

        // Рисуем пули первого игрока
        for (auto& bullet : players[0].GetBullets()) {
            bullet.Draw(hdc);
        }

        // Рисуем пули второго игрока
        for (auto& bullet : players[1].GetBullets()) {
            bullet.Draw(hdc);
        }

        EndPaint(hwnd, &ps);
        break;
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
        RECT updateRect = { 0, 0, 0, 0 }; // Начальная область для перерисовки

        if (wParam == TIMER_ONE_PLAYER_ID) {
            // Сохраняем старую позицию танка
            RECT oldTankRect = GetUnitRect(players[0]);

            // Обновляем позицию танка
            players[0].movement(Map1, players);

            // Добавляем старую и новую позицию танка в область перерисовки
            RECT newTankRect = GetUnitRect(players[0]);
            UnionRect(&updateRect, &updateRect, &oldTankRect);
            UnionRect(&updateRect, &updateRect, &newTankRect);

            // Обработка пуль
            for (size_t i = 0; i < players[0].GetBullets().size(); ) {
                Bullet& bullet = players[0].GetBullets()[i];
                RECT oldBulletRect = bullet.GetRect();

                bullet.Move(Map1, hwnd, players);

                if (!bullet.IsAlive()) {
                    UnionRect(&updateRect, &updateRect, &oldBulletRect);
                    depthBullet.push_back(std::move(bullet));
                    players[0].GetBullets().erase(players[0].GetBullets().begin() + i);
                }
                else {
                    RECT newBulletRect = bullet.GetRect();
                    UnionRect(&updateRect, &updateRect, &oldBulletRect);
                    UnionRect(&updateRect, &updateRect, &newBulletRect);
                    i++;
                }
            }
        }

        if (wParam == TIMER_TWO_PLAYER_ID) {
            // Сохраняем старую позицию танка
            RECT oldTankRect = GetUnitRect(players[1]);

            // Обновляем позицию танка
            if (players[1].isAlive()) {
                players[1].movement(Map1, players);
            }

            // Добавляем старую и новую позицию танка в область перерисовки
            RECT newTankRect = GetUnitRect(players[1]);
            UnionRect(&updateRect, &updateRect, &oldTankRect);
            UnionRect(&updateRect, &updateRect, &newTankRect);

            // Обработка пуль
            for (size_t i = 0; i < players[1].GetBullets().size(); ) {
                Bullet& bullet = players[1].GetBullets()[i];
                RECT oldBulletRect = bullet.GetRect();
                bullet.Move(Map1, hwnd, players);
                RECT newBulletRect = bullet.GetRect();

                UnionRect(&updateRect, &updateRect, &oldBulletRect);
                UnionRect(&updateRect, &updateRect, &newBulletRect);

                if (!bullet.IsAlive()) {
                    UnionRect(&updateRect, &updateRect, &oldBulletRect);
                    depthBullet.push_back(std::move(bullet));
                    players[1].GetBullets().erase(players[1].GetBullets().begin() + i);
                }
                else {
                    i++;
                }
            }
        }

        InvalidateRect(hwnd, &updateRect, FALSE);
        break;
    }
    case WM_KEYDOWN: {
        switch (wParam) {
        case 'W':
            players[0].moveUp = true;
            players[0].moveDown = false;
            players[0].moveLeft = false;
            players[0].moveRight = false;
            players[0].SetAngle(90); // Угол для движения вверх
            break;
        case 'A':
            players[0].moveUp = false;
            players[0].moveDown = false;
            players[0].moveLeft = true;
            players[0].moveRight = false;
            players[0].SetAngle(180); // Угол для движения влево
            break;
        case 'S':
            players[0].moveUp = false;
            players[0].moveDown = true;
            players[0].moveLeft = false;
            players[0].moveRight = false;
            players[0].SetAngle(270); // Угол для движения вниз
            break;
        case 'D':
            players[0].moveUp = false;
            players[0].moveDown = false;
            players[0].moveLeft = false;
            players[0].moveRight = true;
            players[0].SetAngle(0); // Угол для движения вправо
            break;
        case VK_UP:
            players[1].moveUp = true;
            players[1].moveDown = false;
            players[1].moveLeft = false;
            players[1].moveRight = false;
            players[1].SetAngle(90);
            break;
        case VK_DOWN:
            players[1].moveUp = false;
            players[1].moveDown = true;
            players[1].moveLeft = false;
            players[1].moveRight = false;
            players[1].SetAngle(270);
            break;
        case VK_LEFT:
            players[1].moveUp = false;
            players[1].moveDown = false;
            players[1].moveLeft = true;
            players[1].moveRight = false;
            players[1].SetAngle(180);
            break;
        case VK_RIGHT:
            players[1].moveUp = false;
            players[1].moveDown = false;
            players[1].moveLeft = false;
            players[1].moveRight = true;
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
    }
    case WM_KEYUP: {
        switch (wParam)
        {
        case 'W':
            players[0].moveUp = false;
            break;
        case 'A':
            players[0].moveLeft = false;
            break;
        case 'S':
            players[0].moveDown = false;
            break;
        case 'D':
            players[0].moveRight = false;
            break;
        case VK_UP:
            players[1].moveUp = false;
            break;
        case VK_DOWN:
            players[1].moveDown = false;
            break;

        case VK_LEFT:
            players[1].moveLeft = false;
            break;
        case VK_RIGHT:
            players[1].moveRight = false;
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
    FillRect(hdc, &rect, brush);
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

        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++) {
                int type = Block::emptiness;
                if (i == 0 || i == HEIGHT - 1) {
                    type = Block::border;
                }
                if (j == 0 || j == WIDTH - 1) {
                    type = Block::border;
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
            for (int j = 21; j < WIDTH - 3; j++) {
                Map1[i][j].initialization(Block::brick);
                Map1[i][j].positionBlock(i, j);
            }
        }

        Map1[5][16].initialization(Block::brick);
        Map1[5][16].positionBlock(5, 16);

        Map1[5][17].initialization(Block::brick);
        Map1[5][17].positionBlock(5, 17);

        //Map1[HEIGHT - 2][14].initialization(Block::base);

        Map1[HEIGHT - 2][14].initialization(Block::emptiness);
        Map1[HEIGHT - 2][14].positionBlock(HEIGHT - 2, 14);

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
            for (int j = 21; j < WIDTH - 3; j++) {
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
            for (int j = 21; j < WIDTH - 3; j++) {
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
            for (int j = 21; j < WIDTH - 3; j++) {
                Map1[i][j].initialization(Block::brick);
                Map1[i][j].positionBlock(i, j);
            }
        }

        // Сохраняем карту в файл
        ofstream fileMap(mapFilePath, ios::out);
        if (fileMap.is_open()) {
            for (int i = 0; i < HEIGHT; i++) {
                for (int j = 0; j < WIDTH; j++) {
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
            while (getline(fileMap, line) && i < HEIGHT) {
                for (int j = 0; j < line.length() && j < WIDTH; j++) {
                    int blockType = line[j] - '0'; // Конвертируем символ в число
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

void Draw() {
    HBRUSH borderBrush = CreateSolidBrush(RGB(128, 128, 128));
    HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
    HBRUSH brickBrush = CreateSolidBrush(RGB(128, 15, 0));
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (Map1[i][j].getNumberBlock() == Block::border) {
                RECT block;
                block.left = Map1[i][j].Position.x1;
                block.right = Map1[i][j].Position.x2;
                block.top = Map1[i][j].Position.y1;
                block.bottom = Map1[i][j].Position.y2;

                FillRect(hdc, &block, borderBrush);
                FrameRect(hdc, &block, blackBrush);
            }
            if (Map1[i][j].getNumberBlock() == Block::brick) {
                RECT block;
                block.left = Map1[i][j].Position.x1;
                block.right = Map1[i][j].Position.x2 - 8;
                block.top = Map1[i][j].Position.y1;
                block.bottom = Map1[i][j].Position.y2 - 8;

                FillRect(hdc, &block, brickBrush);
                FrameRect(hdc, &block, blackBrush);

                block.left = Map1[i][j].Position.x1;
                block.right = Map1[i][j].Position.x2 - 8;
                block.top = Map1[i][j].Position.y1 + 8;
                block.bottom = Map1[i][j].Position.y2;

                FillRect(hdc, &block, brickBrush);
                FrameRect(hdc, &block, blackBrush);

                block.left = Map1[i][j].Position.x1 + 8;
                block.right = Map1[i][j].Position.x2;
                block.top = Map1[i][j].Position.y1 + 8;
                block.bottom = Map1[i][j].Position.y2;

                FillRect(hdc, &block, brickBrush);
                FrameRect(hdc, &block, blackBrush);

                block.left = Map1[i][j].Position.x1 + 8;
                block.right = Map1[i][j].Position.x2;
                block.top = Map1[i][j].Position.y1;
                block.bottom = Map1[i][j].Position.y2 - 8;

                FillRect(hdc, &block, brickBrush);
                FrameRect(hdc, &block, blackBrush);
            }
        }
    }
}

void drawUnit(HWND hwnd, Tank& player, HBRUSH brush) {
    HBRUSH blackBrush = CreateSolidBrush(RGB(60, 60, 60));
    // 1. Очищаем предыдущее положение (и танк, и пушку)
    if (player.position.x1 != player.tempPosition.x1 || player.position.y1 != player.tempPosition.y1)
    {
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, fieldBrush);
        HPEN oldPen = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN));

        // Очищаем область танка с запасом для пушки
        Rectangle(hdc,
            player.tempPosition.x1 - 1,  // Запас для пушки
            player.tempPosition.y1 - 1,
            player.tempPosition.x2 + 1,
            player.tempPosition.y2 + 1);

        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
    }

    // 2. Рисуем танк
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
    HPEN oldPen = (HPEN)SelectObject(hdc, tankPen);

    if (!player.isAlive()) {
        oldBrush = (HBRUSH)SelectObject(hdc, blackBrush);
    }

    // Корпус танка
    Rectangle(hdc,
        player.position.x1,
        player.position.y1,
        player.position.x2,
        player.position.y2);

    // Внутренний прямоугольник танка
    Rectangle(hdc,
        player.position.x1 + 4,
        player.position.y1 + 4,
        player.position.x2 - 4,
        player.position.y2 - 4);

    // 3. Рисуем пушку
    int centerX = (player.position.x1 + player.position.x2) / 2;
    int centerY = (player.position.y1 + player.position.y2) / 2;
    double angleRad = player.GetAngle() * M_PI / 180.0;
    int gunLength = 7;
    int gunEndX = centerX + static_cast<int>(gunLength * cos(angleRad));
    int gunEndY = centerY - static_cast<int>(gunLength * sin(angleRad));

    // Создаем перо для пушки
    HPEN gunPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));  // Более толстая пушка
    HPEN oldGunPen = (HPEN)SelectObject(hdc, gunPen);

    // Рисуем пушку
    MoveToEx(hdc, centerX, centerY, NULL);
    LineTo(hdc, gunEndX, gunEndY);

    // Восстанавливаем ресурсы
    SelectObject(hdc, oldGunPen);
    DeleteObject(gunPen);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);

    // Обновляем временную позицию
    player.tempPosition = player.position;
}
