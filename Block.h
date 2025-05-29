#pragma once
#include <Windows.h>
class Block{
public:
	Block();

	void damage(int damage, HWND hwnd);
	void initialization(int type);
	void positionBlock(int i, int j);
	int getNumberBlock();

	static const int emptiness = 0;
	static const int border = 1;
	static const int brick = 2;
	static const int whiteBrick = 3;
	static const int foliage = 4;
	static const int base = 5;

	struct position {
		int x1;
		int y1;
		int x2;
		int y2;
	};

	position Position;
private:
	int blockNumber;
	int endurance;
};

