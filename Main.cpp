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

Tank* players = new Tank[2];

/*Function*/

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static int sX, sY;
    static Tank playerOne, playerTwo;
    switch (uMsg) {
    case WM_SIZE:
        sX = LOWORD(lParam);
        sY = LOWORD(wParam);
        break; // Add break here

    case WM_CREATE:
        createTank(playerOne, 16 * 16 + shift, 28 * 16 + shift, 16 * 17 + shift, 29 * 16 + shift); // Зеленый танк внизу
        createTank(playerTwo, 16 * 16 + shift, 2 * 16 + shift, 17 * 16 + shift, 3 * 16 + shift);  // Красный танк вверху
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
        drawUnit(hwnd, playerOne, greenBrush);
        drawUnit(hwnd, playerTwo, redBrush);

        // Рисуем пули первого игрока
        for (auto& bullet : playerOne.GetBullets()) {
            bullet.Draw(hdc);
        }

        // Рисуем пули второго игрока
        for (auto& bullet : playerTwo.GetBullets()) {
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
    case WM_TIMER:
        if (wParam == TIMER_ONE_PLAYER_ID) {
            RECT oldRect = GetUnitRect(playerOne);
            playerOne.movement(Map1);

            RECT newRectOne = GetUnitRect(playerOne);

            RECT updateRectOne;
            UnionRect(&updateRectOne, &oldRect, &newRectOne);

            for (size_t i = 0; i < playerOne.GetBullets().size(); ) {
                playerOne.GetBullets()[i].Move(Map1, hwnd);
                RECT bulletRect = playerOne.GetBullets()[i].GetRect(); //Сохраняем позицию до удаления
                UnionRect(&updateRectOne, &updateRectOne, &bulletRect);
                if (!playerOne.GetBullets()[i].IsAlive()) {
                    playerOne.GetBullets().erase(playerOne.GetBullets().begin() + i);
                }
                else {
                    playerOne.GetBullets()[i].Draw(hdc);
                    ++i;
                }
            }
            InvalidateRect(hwnd, &updateRectOne, FALSE);
        }
        if (wParam == TIMER_TWO_PLAYER_ID) {
            RECT oldRect = GetUnitRect(playerTwo);
            playerTwo.movement(Map1);
            RECT newRectTwo = GetUnitRect(playerTwo);

            RECT updateRectTwo;
            UnionRect(&updateRectTwo, &oldRect, &newRectTwo);

            for (size_t i = 0; i < playerTwo.GetBullets().size(); ) {
                playerTwo.GetBullets()[i].Move(Map1, hwnd);
                RECT bulletRect = playerTwo.GetBullets()[i].GetRect();
                UnionRect(&updateRectTwo, &updateRectTwo, &bulletRect);
                if (!playerTwo.GetBullets()[i].IsAlive()) {
                    playerTwo.GetBullets().erase(playerTwo.GetBullets().begin() + i);
                }
                else {
                    playerTwo.GetBullets()[i].Draw(hdc);
                    ++i;
                }
            }
            InvalidateRect(hwnd, &updateRectTwo, FALSE);
        }
        break;

    case WM_KEYDOWN: {
        switch (wParam) {
        case 'W':
            playerOne.moveUp = true;
            playerOne.moveDown = false;
            playerOne.moveLeft = false;
            playerOne.moveRight = false;
            playerOne.SetAngle(90); //смотрим вверх
            break;
        case 'A':
            playerOne.moveLeft = true;
            playerOne.moveDown = false;
            playerOne.moveUp = false;
            playerOne.moveRight = false;
            playerOne.SetAngle(180); //смотрим влево
            break;
        case 'S':
            playerOne.moveDown = true;
            playerOne.moveUp = false;
            playerOne.moveLeft = false;
            playerOne.moveRight = false;
            playerOne.SetAngle(270); //смотрим вниз
            break;
        case 'D':
            playerOne.moveRight = true;
            playerOne.moveDown = false;
            playerOne.moveUp = false;
            playerOne.moveLeft = false;
            playerOne.SetAngle(0); //смотрим вправо
            break;
        case VK_UP:
            playerTwo.moveUp = true;
            playerTwo.moveDown = false;
            playerTwo.moveLeft = false;
            playerTwo.moveRight = false;
            playerTwo.SetAngle(90); //смотрим вверх
            break;
        case VK_DOWN:
            playerTwo.moveDown = true;
            playerTwo.moveUp = false;
            playerTwo.moveLeft = false;
            playerTwo.moveRight = false;
            playerTwo.SetAngle(270); //смотрим вверх
            break;
        case VK_LEFT:
            playerTwo.moveLeft = true;
            playerTwo.moveDown = false;
            playerTwo.moveUp = false;
            playerTwo.moveRight = false;
            playerTwo.SetAngle(180); //смотрим вверх
            break;
        case VK_RIGHT:
            playerTwo.moveRight = true;
            playerTwo.moveDown = false;
            playerTwo.moveUp = false;
            playerTwo.moveLeft = false;
            playerTwo.SetAngle(0); //смотрим вверх
            break;
        case VK_SPACE: {
            playerOne.Shoot(Map1); // Стрельба по пробелу
            break;
        }
        case VK_SHIFT: {
            playerTwo.Shoot(Map1); // Стрельба по правому шифту
            break;
        }

        default:
            break;
        }
        // Запрос на перерисовку только области юнита
        RECT updateRect1 = GetUnitRect(playerOne);
        RECT updateRect2 = GetUnitRect(playerTwo);
        UpdateWindow(hwnd); // Добавьте эту строку
        break;
    }
    case WM_KEYUP: {
        switch (wParam)
        {
        case 'W':
            playerOne.moveUp = false;
            break;
        case 'A':
            playerOne.moveLeft = false;
            break;
        case 'S':
            playerOne.moveDown = false;
            break;
        case 'D':
            playerOne.moveRight = false;
            break;
        case VK_UP:
            playerTwo.moveUp = false;
            break;
        case VK_DOWN:
            playerTwo.moveDown = false;
            break;

        case VK_LEFT:
            playerTwo.moveLeft = false;
            break;
        case VK_RIGHT:
            playerTwo.moveRight = false;
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

void createTank(Tank &player, int x1, int y1, int x2, int y2) {
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

        Map1[HEIGHT - 2][14].initialization(Block::base);
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
    // 1. Очищаем предыдущее положение (и танк, и пушку)
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
