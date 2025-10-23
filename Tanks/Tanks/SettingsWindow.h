#pragma once
#ifndef SETTINGS_H
#define SETTINGS_H

#include <Windows.h>
#include "Tank.h"
#include <vector>

// Идентификаторы элементов управления
#define ID_EDIT_HP1 2001
#define ID_EDIT_DAMAGE1 2002
#define ID_EDIT_HP2 2003
#define ID_EDIT_DAMAGE2 2004
#define ID_BUTTON_SAVE 2005
#define ID_BUTTON_CANCEL 2006

// Внешние объявления
extern std::vector<Tank> players;
extern int massiveStartHP[2];
extern int massiveStartDamage[2];

LRESULT CALLBACK WindowProcSettings(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif