#include "Block.h"
#include <iostream>

using namespace std;

void Block::initialization(int y, int x){
	switch (blockNumber)
	{
	case border:
		endurance = INT_MAX;
		break;
	case brick:
		endurance = 50;
		break;
	case whiteBrick:
		endurance = INT_MAX;
		break;
	case foliage:
		endurance = INT_MAX;
		break;
	case base:
		endurance = 250;
		break;
	case emptiness:
		endurance = INT_MAX;
		break;
	default:
		break;
	}
}

void Block::damage(int damage) {
	if (blockNumber == brick || blockNumber == base) {
		endurance -= damage;
		if (endurance <= 0) {
			blockNumber = emptiness;
			endurance = INT_MAX;
		}
	}
}
