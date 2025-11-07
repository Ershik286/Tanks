#pragma once
#ifndef MENU_H
#define MENU_H

#include <Windows.h>

// Идентификаторы кнопок
#define ID_BUTTON_PLAY 1001
#define ID_BUTTON_SETTINGS 1002
#define ID_BUTTON_EXIT 1003

// Процедура окна меню
LRESULT CALLBACK WindowProcMenu(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif