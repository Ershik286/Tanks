#include "Tank.h"
#include "Block.h"
#include <iostream>  // Для отладки

using namespace std;

Tank::Tank() : health(100), damage(10), speed (5){
    moveUp = moveDown = moveLeft = moveRight = false;
    position.x1 = position.y1 = -100; // Начальная позиция вне экрана
    position.x2 = position.y2 = -100 + 16; // Начальная позиция вне экрана
    tempPosition = position;
}

void Tank::movement(Block Map[HEIGHT][WIDTH]) {

    tempPosition = position;

    if (moveUp && position.y1 - speed >= 0) {
        position.y1 -= speed;
        position.y2 -= speed;
    }
    if (moveDown && position.y1 + speed + 16 <= HEIGHT * 16){
        position.y1 += speed;
        position.y2 += speed;
    }
    if (moveLeft && position.x1 - speed >= 0) {
        position.x1 -= speed;
        position.x2 -= speed;
    }
    if (moveRight && position.x1 + speed + 16 <= WIDTH * 16){
        position.x1 += speed;
        position.x2 += speed;
    }
    if (checkMove(position, Map)) {
        position = tempPosition;
    }
}

//ToDo
bool Tank::checkMove(Position pos, Block Map[HEIGHT][WIDTH]) {

    if (Map[pos.y1 / 16 + 1][pos.x1 / 16 + 1].blockNumber != Block::emptiness && Map[pos.y1 / 16 + 1][pos.x1 / 16 + 1].blockNumber != Block::foliage)
        return true;

    return false;
}

void Tank::shoot() {

}

void Tank::changeOfDirection() {
}

void Tank::treatment() {

}
