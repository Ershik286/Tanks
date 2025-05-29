#include "Block.h"
#include <iostream>
#include <Windows.h>

using namespace std;

Block::Block(){}

void Block::initialization(int type){
	this->blockNumber = type;
	switch (blockNumber)
	{
	case border:
		this->endurance = INT_MAX;
		break;
	case brick:
		this->endurance = 30;
		break;
	case whiteBrick:
		this->endurance = INT_MAX;
		break;
	case foliage:
		this->endurance = INT_MAX;
		break;
	case base:
		this->endurance = 250;
		break;
	case emptiness:
		this->endurance = INT_MAX;
		break;
	default:
		break;
	}
}

void Block::positionBlock(int i, int j) {
	Position.x1 = j * 16;
	Position.y1 = i * 16;
	Position.x2 = (j + 1) * 16;
	Position.y2 = (i + 1) * 16;
}

int Block::getNumberBlock(){
	return blockNumber;
}

void Block::damage(int damage, HWND hwnd) {
	if (blockNumber == brick || blockNumber == base) {
		endurance -= damage;
		if (endurance <= 0) {
			this->blockNumber = emptiness;
			endurance = INT_MAX;
			RECT areaBlockNull;
			areaBlockNull.left = Position.x1;
			areaBlockNull.right = Position.x2;
			areaBlockNull.top = Position.y1;
			areaBlockNull.bottom = Position.y2;
			InvalidateRect(hwnd, &areaBlockNull, TRUE);
		}
	}
}


