#include "Block.h"
#include <iostream>
#include <Windows.h>

using namespace std;

Block::Block(){
	this->endurance = 1;
	this->blockNumber = 0;
	this->destroyed = false;
	Position = {0, 0, 0, 0};
}

void Block::initialization(int type){
	this->blockNumber = type;
	switch (blockNumber)
	{
	case border || whiteBrick || foliage || emptiness:
		this->endurance = INT_MAX;
		break;
	case brick:
		this->endurance = 30;
		break;
	case base:
		this->endurance = 250;
		break;
	default:
		break;
	}
	if (blockNumber == healthBox || blockNumber == damageUpBox || blockNumber == speedUpBox) {
		this->endurance = 1;
	}
}

void Block::positionBlock(int i, int j) {
	this->Position.x1 = j * 16;
	this->Position.y1 = i * 16;
	this->Position.x2 = (j + 1) * 16;
	this->Position.y2 = (i + 1) * 16;
}

int Block::getNumberBlock(){
	return blockNumber;
}

void Block::damage(int damageAmount, HWND hwnd) {
	this->endurance -= damageAmount; // reduce the endurance

	// When an object is dead, then it will perform destruction
	if (endurance <= 0) {
		initialization(emptiness);

		// Now invalidate the previous rectangle, so that it can be redrawn
		RECT areaBlockNull;
		areaBlockNull.left = Position.x1;
		areaBlockNull.right = Position.x2;
		areaBlockNull.top = Position.y1;
		areaBlockNull.bottom = Position.y2;

		InvalidateRect(hwnd, &areaBlockNull, FALSE); // Re-draw the block
	}
}

bool Block::isDestroyed() const {
	return destroyed;
}
