#include "SettingsWindow.h"
#include <Windows.h>
#include <iostream>	
#include <string>
#include <wchar.h>
#include <cwchar>
#include <vector>
#include "Main.h"

using namespace std;

const int WIDTH = 400;
const int HEIGHT = 300;

HDC settingHDC;

LRESULT CALLBACK WindowProcSettings(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hEditHP1, hEditDamage1, hEditHP2, hEditDamage2;
    static HWND hButtonSave, hButtonCancel;
    static HFONT hFont;

    switch (uMsg) {
    case WM_CREATE:
        // Создаем статические тексты
        CreateWindowW(L"STATIC", L"Танк 1:",
            WS_VISIBLE | WS_CHILD,
            20, 20, 100, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);

        CreateWindowW(L"STATIC", L"HP:",
            WS_VISIBLE | WS_CHILD,
            40, 50, 40, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);

        CreateWindowW(L"STATIC", L"Урон:",
            WS_VISIBLE | WS_CHILD,
            40, 80, 40, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);

        CreateWindowW(L"STATIC", L"Танк 2:",
            WS_VISIBLE | WS_CHILD,
            20, 120, 100, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);

        CreateWindowW(L"STATIC", L"HP:",
            WS_VISIBLE | WS_CHILD,
            40, 150, 40, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);

        CreateWindowW(L"STATIC", L"Урон:",
            WS_VISIBLE | WS_CHILD,
            40, 180, 40, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);

        // Создаем TextBox'ы для танка 1
        hEditHP1 = CreateWindowW(L"EDIT", L"100",
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
            90, 50, 80, 20, hwnd, (HMENU)ID_EDIT_HP1, GetModuleHandle(NULL), NULL);

        hEditDamage1 = CreateWindowW(L"EDIT", L"10",
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
            90, 80, 80, 20, hwnd, (HMENU)ID_EDIT_DAMAGE1, GetModuleHandle(NULL), NULL);

        // Создаем TextBox'ы для танка 2
        hEditHP2 = CreateWindowW(L"EDIT", L"100",
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
            90, 150, 80, 20, hwnd, (HMENU)ID_EDIT_HP2, GetModuleHandle(NULL), NULL);

        hEditDamage2 = CreateWindowW(L"EDIT", L"10",
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
            90, 180, 80, 20, hwnd, (HMENU)ID_EDIT_DAMAGE2, GetModuleHandle(NULL), NULL);

        // Создаем кнопки
        hButtonSave = CreateWindowW(L"BUTTON", L"Сохранить",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            50, 220, 100, 30, hwnd, (HMENU)ID_BUTTON_SAVE, GetModuleHandle(NULL), NULL);

        hButtonCancel = CreateWindowW(L"BUTTON", L"Отмена",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            170, 220, 100, 30, hwnd, (HMENU)ID_BUTTON_CANCEL, GetModuleHandle(NULL), NULL);

        // Устанавливаем шрифт
        hFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");

        if (hFont) {
            SendMessage(hEditHP1, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hEditDamage1, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hEditHP2, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hEditDamage2, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hButtonSave, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hButtonCancel, WM_SETFONT, (WPARAM)hFont, TRUE);
        }

        // Загружаем текущие значения из players (если они существуют)
        if (players.size() >= 2) {
            wstring hp1 = to_wstring(players[0].GetHealth());
            wstring damage1 = to_wstring(players[0].GetDamage()); // Нужно добавить GetDamage() в класс Tank
            wstring hp2 = to_wstring(players[1].GetHealth());
            wstring damage2 = to_wstring(players[1].GetDamage());

            SetWindowText(hEditHP1, hp1.c_str());
            SetWindowText(hEditDamage1, damage1.c_str());
            SetWindowText(hEditHP2, hp2.c_str());
            SetWindowText(hEditDamage2, damage2.c_str());
        }
        break;

    case WM_COMMAND: {
        int wmId = LOWORD(wParam);

        switch (wmId) {
        case ID_BUTTON_SAVE: {
            wchar_t buffer[10];

            // Читаем значения из TextBox'ов
            GetWindowText(hEditHP1, buffer, 10);
            int hp1 = _wtoi(buffer);

            GetWindowText(hEditDamage1, buffer, 10);
            int damage1 = _wtoi(buffer);

            GetWindowText(hEditHP2, buffer, 10);
            int hp2 = _wtoi(buffer);

            GetWindowText(hEditDamage2, buffer, 10);
            int damage2 = _wtoi(buffer);

            // Применяем настройки к танкам
            massiveStartHP[0] = hp1 > 0 ? hp1 : 1;
            massiveStartDamage[0] = damage1 ? damage1 : 1;

            massiveStartHP[1] = hp2 > 0 ? hp2 : 1;
            massiveStartDamage[1] = damage2 > 0 ? hp2 : 1;

            MessageBoxW(hwnd, L"Настройки сохранены!", L"Успех", MB_OK | MB_ICONINFORMATION);
            DestroyWindow(hwnd);
            break;
        }
        case ID_BUTTON_CANCEL:
            DestroyWindow(hwnd);
            break;
        }
        break;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        settingHDC = BeginPaint(hwnd, &ps);

        // Заливаем фон
        RECT rect;
        GetClientRect(hwnd, &rect);
        FillRect(settingHDC, &rect, (HBRUSH)(COLOR_WINDOW + 1));

        // Заголовок окна
        SetBkMode(settingHDC, TRANSPARENT);
        SetTextColor(settingHDC, RGB(0, 0, 0));
        DrawText(settingHDC, L"Настройки танков", -1, &rect,
            DT_CENTER | DT_TOP | DT_SINGLELINE);

        EndPaint(hwnd, &ps);
        break;
    }

    case WM_DESTROY:
        // Освобождаем шрифт
        if (hFont) {
            DeleteObject(hFont);
        }
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}