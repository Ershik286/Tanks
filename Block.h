#pragma once
class Block{
public:

	void damage(int damage);
	void initialization(int y, int x);

	int blockNumber;
	static const int emptiness = 0;
	static const int border = 1;
	static const int brick = 2;
	static const int whiteBrick = 3;
	static const int foliage = 4;
	static const int base = 5;

private:
	struct position {
		int x;
		int y;
	};
	int endurance;
};

