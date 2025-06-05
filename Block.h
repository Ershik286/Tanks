#pragma once
#include <Windows.h>
class Block{
public:
	Block();

	void damage(int damageAmount, HWND hwnd);
	bool isDestroyed() const;
	void initialization(int type);
	void positionBlock(int i, int j);
	int getNumberBlock();

	static const int emptiness = 0;
	static const int border = 1;
	static const int brick = 2;
	static const int whiteBrick = 3;
	static const int foliage = 4;
	static const int base = 5;
	static const int healthBox = 6;
	static const int damageUpBox = 7;
	static const int speedUpBox = 8;

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
	bool destroyed;
};

