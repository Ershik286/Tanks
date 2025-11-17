// Menu.h
#pragma once
#ifndef MENU_H
#define MENU_H

#include <Windows.h>

#ifdef MENU_EXPORTS
#define MENU_API __declspec(dllexport)
#else
#define MENU_API __declspec(dllimport)
#endif

#define ID_BUTTON_PLAY 1001
#define ID_BUTTON_SETTINGS 1002
#define ID_BUTTON_EXIT 1003

extern "C" {
    MENU_API HWND CreateMenuWindow(HINSTANCE hInstance, HWND hParent, HWND hGameWindow, HWND hSettingsWindow);
    MENU_API LRESULT CALLBACK WindowProcMenu(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}

#endif
