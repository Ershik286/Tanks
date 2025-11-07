#include <Windows.h>
#include <iostream>
#include <string>
#include <wchar.h>
#include <cwchar>
#include "Menu.h"
#include "Main.h"

using namespace std;

const int WIDTH = 300;
const int HEIGHT = 200;

// Идентификаторы кнопок
#define ID_BUTTON_PLAY 1001
#define ID_BUTTON_SETTINGS 1002
#define ID_BUTTON_EXIT 1003

HDC hdcNew;

LRESULT CALLBACK WindowProcMenu(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static int sX, sY;
    static HWND hButtonPlay, hButtonSettings, hButtonExit;
    HFONT hFont;
    switch (uMsg) {
    case WM_CREATE:

        // Создаем кнопки
        hButtonPlay = CreateWindowW(
            L"BUTTON", L"Играть",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            50, 50, 200, 40,
            hwnd, (HMENU)ID_BUTTON_PLAY,
            GetModuleHandle(NULL), NULL
        );

        hButtonSettings = CreateWindowW(
            L"BUTTON", L"Настройки",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            50, 100, 200, 40,
            hwnd, (HMENU)ID_BUTTON_SETTINGS,
            GetModuleHandle(NULL), NULL
        );

        hButtonExit = CreateWindowW(
            L"BUTTON", L"Выйти",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            50, 150, 200, 40,
            hwnd, (HMENU)ID_BUTTON_EXIT,
            GetModuleHandle(NULL), NULL
        );

        // Устанавливаем шрифт для кнопок (опционально)
        hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");

        if (hFont) {
            SendMessage(hButtonPlay, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hButtonSettings, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hButtonExit, WM_SETFONT, (WPARAM)hFont, TRUE);
        }
        break;

    case WM_SIZE:
        sX = LOWORD(lParam);
        sY = HIWORD(lParam); // Исправлено: должно быть HIWORD для высоты

        // Можно добавить обработку изменения размера окна для кнопок
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        hdcNew = BeginPaint(hwnd, &ps);

        // Заливаем фон
        RECT rect;
        GetClientRect(hwnd, &rect);
        FillRect(hdcNew, &rect, (HBRUSH)(COLOR_WINDOW + 1));

        // Можно добавить текст или графику
        SetBkMode(hdcNew, TRANSPARENT);
        SetTextColor(hdcNew, RGB(0, 0, 0));

        // Заголовок меню
        DrawText(hdcNew, L"Главное меню", -1, &rect,
            DT_CENTER | DT_TOP | DT_SINGLELINE);

        EndPaint(hwnd, &ps);
        break;
    }

    case WM_COMMAND: {
        int wmId = LOWORD(wParam);

        switch (wmId) {
        case ID_BUTTON_PLAY:
            
            ShowWindow(gameWindow, 1); 
            break;

        case ID_BUTTON_SETTINGS:

            ShowWindow(settingsWindow, 1);
            break;

        case ID_BUTTON_EXIT:
            if (MessageBoxW(hwnd, L"Вы уверены, что хотите выйти?", L"Выход",
                MB_YESNO | MB_ICONQUESTION) == IDYES) {
                DestroyWindow(hwnd);
            }
            break;
        }
        break;
    }

    case WM_LBUTTONDOWN: {
        int xPos = LOWORD(lParam);
        int yPos = HIWORD(lParam);
        wstring positionClick = to_wstring(xPos) + L"/" + to_wstring(yPos);
        LPCWSTR positionClickLPC = positionClick.c_str();
        MessageBoxW(hwnd, positionClickLPC, L"Координаты", MB_OK);
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}