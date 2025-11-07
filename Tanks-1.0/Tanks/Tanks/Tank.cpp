#include "Tank.h"
#include "Block.h"
#include "Bullet.h"
#include "Main.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <Windows.h>

using namespace std;

#define M_PI 3.14

Tank::Tank(bool isAI) : health(100), damage(10), speed(4), angle(90) {
    for (int i = 0; i < 4; i++)
        movingVector[i] = 0;

    position.x1 = position.y1 = -100; // Начальная позиция вне экрана
    position.x2 = position.y2 = -100 + 16; // Начальная позиция вне экрана
    tempPosition = position;
    tempAngle = 0;
    blockPositionX = 0; //заготовка
    blockPositionY = 0;
    live = true;
    tempHealth = health;
    this->isAI = isAI;
}

void Tank::movement(Block Map[HEIGHT][WIDTH], vector<Tank>& tanks, HWND hwnd) { // Передаем вектор танков
    tempPosition = position;

    if (movingVector[1] && position.y1 - speed >= 0) {
        position.y1 -= speed;
        position.y2 -= speed;
    }
    if (movingVector[3] && position.y1 + speed + 16 <= HEIGHT * 16) {
        position.y1 += speed;
        position.y2 += speed;
    }
    if (movingVector[2] && position.x1 - speed >= 0) {
        position.x1 -= speed;
        position.x2 -= speed;
    }
    if (movingVector[0] && position.x1 + speed + 16 <= WIDTH * 16) {
        position.x1 += speed;
        position.x2 += speed;
    }
    if (checkMove(position, Map, hwnd)) {
        position = tempPosition;
    }

    // Проверка на столкновение с другими танками
    for (Tank& otherTank : tanks) {
        if (this != &otherTank) { // Не проверяем столкновение с самим собой
            if (isColliding(position, otherTank.position)) {
                position = tempPosition; // Откатываем движение, если есть столкновение
                break; // Прерываем цикл, если столкновение уже обнаружено
            }
        }
    }
}

bool Tank::isColliding(Position pos1, Position pos2) const {
    return !(pos1.x2 < pos2.x1 || pos1.x1 > pos2.x2 || pos1.y2 < pos2.y1 || pos1.y1 > pos2.y2);
}

int Tank::GetDamage() {
    return damage;
}

bool Tank::checkMove(Position pos, Block Map[HEIGHT][WIDTH], HWND hwnd) {
    const int buffer = 2; // Например, 2 пикселя

    const pair<int, int> checkPoints[] = {
        {pos.x1 + buffer, pos.y1 + buffer},
        {pos.x2 - buffer, pos.y1 + buffer},
        {pos.x1 + buffer, pos.y2 - buffer},
        {pos.x2 - buffer, pos.y2 - buffer},
        {(pos.x1 + pos.x2) / 2, (pos.y1 + pos.y2) / 2}
    };

    bool collisionDetected = false;

    for (const auto& point : checkPoints) {
        int x = point.first;
        int y = point.second;

        int blockX = x / 16;
        int blockY = y / 16;

        if (blockX < 0 || blockX >= WIDTH || blockY < 0 || blockY >= HEIGHT) {
            return true;
        }

        int blockType = Map[blockY][blockX].getNumberBlock();

        if (blockType == Block::healthBox) {
            SetHealth(GetHealth() + 20); // Увеличиваем здоровье танка
            Map[blockY][blockX].damage(100, hwnd); // Уничтожаем блок
            continue; // Продолжаем проверку других точек
        }

        if (blockType == Block::damageUpBox || blockType == Block::speedUpBox) {
            Map[blockY][blockX].damage(100, hwnd); // Уничтожаем блок
            continue; // Продолжаем проверку других точек
        }

        if (blockType != Block::emptiness && blockType != Block::foliage) {
            collisionDetected = true;
            break;
        }
    }

    return collisionDetected;
}

vector<Bullet>& Tank::GetBullets() {
    return bullets;
}

void Tank::Shoot(Block Map[HEIGHT][WIDTH]) {
    if (bullets.size() >= 3) return;
    if (!isAlive()) return;

    int startX = position.x1 + (position.x2 - position.x1) / 2;
    int startY = position.y1 + (position.y2 - position.y1) / 2;

    double angleRad = angle * M_PI / 180.0;
    int offsetX = static_cast<int>(10 * cos(angleRad));
    int offsetY = static_cast<int>(10 * sin(angleRad));

    bullets.emplace_back(startX + offsetX, startY - offsetY, angle, 10, this);
}

bool Tank::damageThis(int damage) {
    if (!live) return true; // Уже уничтожен
    this->tempHealth = health;

    this->health -= damage;
    if (health <= 0) {
        this->health = 0;
        this->live = false;
        return true;
    }
    return false;
}

void Tank::SetAngle(int NewAngle) {
    if (!isAlive()) return;
    this->tempAngle = angle;
    this->angle = NewAngle;
}

void Tank::setVectorMoving(int massive[4]){
    for (int i = 0; i < 4; i++) {
        this->movingVector[i] = massive[i];
    }
}

int Tank::GetAngle() const {
    return angle;
}

int Tank::GetTempAngle() const {
    return tempAngle;
}

int Tank::GetHealth() const {
    return health;
}

int Tank::GetTempHealth() const{
    return tempHealth;
}

void Tank::SetHealth(int newHealth) {
    this->tempHealth = health;
    this->health = newHealth;
    health = health > 100 ? 100 : health;
}

void Tank::mainAI(vector<Tank>& tanks, Block Map[HEIGHT][WIDTH], HWND hwnd) {
    if (!isAI) return;

    // Случайное изменение направления каждые N кадров (5% шанс)
    if (rand() % 100 < 3) {
        // Сбрасываем все направления
        for (int i = 0; i < 4; i++) movingVector[i] = 0;

        // Выбираем случайное направление
        int direction = rand() % 4; // 0-3

        movingVector[direction] = 1;

        // Устанавливаем угол в зависимости от направления
        switch (direction) {
        case 0: SetAngle(0); break;
        case 1: SetAngle(90); break;
        case 2: SetAngle(180); break;
        case 3: SetAngle(270); break;
        }
    }

    // Случайная стрельба (3% шанс каждый кадр)
    if (rand() % 100 < 3) {
        Shoot(Map);
    }
}

void Tank::setDamage(int newDamage) {
    this->damage = newDamage;
}

// Остальные методы класса Tank...
bool Tank::isAlive() const {
    return live;
}
