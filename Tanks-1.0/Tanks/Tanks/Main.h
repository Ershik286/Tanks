#pragma once
#ifndef MAIN_H
#define MAIN_H

#include <Windows.h>
#include "Tank.h"
#include "Block.h"
#include <iostream>
#include <vector>

#define TIMER_ONE_PLAYER_ID 0
#define TIMER_TWO_PLAYER_ID 1
#define TIMER_THREE_PLAYER_ID 2
#define TIMER_FOUR_PLAYER_ID 3

#define TIMER_INTERVAL 30
#define TIMER_ON
#define M_PI 3.14

#define IDC_COMBOBOX_PLAYERS 10
#define IDC_BUTTON_START_GAME 11

//vector<Tank> players;
//vector<int> points;
//vector<Bullet> depthBullet{};

extern HWND gameWindow;
extern HWND menuWindow;
extern HWND settingsWindow;

// Процедура окна меню
LRESULT CALLBACK GameWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow);

void drawUnit(HWND hwnd, Tank& player, HBRUSH brush);
RECT GetUnitRect(Tank& player);
void createOneLevel();
void Draw(HDC hdc, const RECT& drawArea, HWND hwnd);
//RECT oldUnitRect; //Unused
void ClearBackGround(HDC hdc, HWND hwnd, RECT rect);
void createTank(Tank& player, int x1, int y1, int x2, int y2);
void DrawHealthBar(HDC hdc, int x, int y, int health, int maxHealth, HBRUSH barBrush);
void drawBlock(RECT& blockRect, Block block);
void drawUnit(HWND hwnd, Tank& player, HBRUSH brush);
bool isColliding(Tank::Position pos1, Block::position pos2);
void createMenu(HWND hwnd);
#endif
