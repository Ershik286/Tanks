#include <iostream>
#include "Block.h"
class Tank {
public:

    struct Position {
        int y1;
        int x1;

        int y2;
        int x2;
    };

    Position position;
    Position tempPosition;

    static const int Up = 0;
    static const int Right = 1;
    static const int Down = 2;
    static const int Left = 3;
    static const int WIDTH = 30;
    static const int HEIGHT = 32;

    bool moveUp;
    bool moveDown;
    bool moveLeft;
    bool moveRight;

    void movement(Block Map[HEIGHT][WIDTH]);
    void shoot();
    void changeOfDirection();
    void treatment();
    bool checkMove(Position pos, Block Map[HEIGHT][WIDTH]);

    Tank();  // Объявление конструктора

protected:

    int speed;
    int health;
    int damage;
};