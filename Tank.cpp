#include "Tank.h"
#include "Block.h"
#include <iostream>

using namespace std;

Tank::Tank() : health(100), damage(10), speed (5){
    moveUp = moveDown = moveLeft = moveRight = false;
    position.x1 = position.y1 = -100; // Начальная позиция вне экрана
    position.x2 = position.y2 = -100 + 16; // Начальная позиция вне экрана
    tempPosition = position;
    blockPositionX = 0; //заготовка
    blockPositionY = 0;
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

bool Tank::checkMove(Position pos, Block Map[HEIGHT][WIDTH]) {
    // Проверяем все 4 угла танка + центр
    const pair<int, int> checkPoints[] = {
        {pos.x1, pos.y1},     // левый верхний угол
        {pos.x2, pos.y1},     // правый верхний угол
        {pos.x1, pos.y2},     // левый нижний угол
        {pos.x2, pos.y2},     // правый нижний угол
        {(pos.x1 + pos.x2) / 2, (pos.y1 + pos.y2) / 2}  // центр
    };

    for (const auto& point : checkPoints) {
        int x = point.first;
        int y = point.second;

        // Переводим координаты в блоки
        int blockX = x / 16;
        int blockY = y / 16;

        // Проверяем границы массива
        if (blockX < 0 || blockX >= WIDTH || blockY < 0 || blockY >= HEIGHT) {
            return true; // Считаем выход за границы непроходимым
        }

        // Проверяем тип блока
        int blockType = Map[blockY][blockX].blockNumber;
        if (blockType != Block::emptiness && blockType != Block::foliage) {
            return true; // Столкновение с непроходимым блоком
        }
    }

    return false; // Все проверяемые точки проходимы
}

void Tank::shoot() {

}

void Tank::changeOfDirection() {

}

void Tank::treatment() {

}
