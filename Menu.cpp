#include <iostream>
#include <Windows.h>
#include <vector>
#include "Menu.h"

void drawMenu(HDC hdc){

}

void functionality(){

}

void mainMenu(HDC hdc) {

	int centerY = 110;
	int centerX = 250;
	int sizeX = 150;
	int sizeY = 50;

	RECT button;
	button.left = centerX - sizeX;
	button.right = centerX + sizeX;

	button.top = centerY - sizeY;
	button.bottom = centerY + sizeY;
}
