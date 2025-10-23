#pragma once
#include <iostream>
#include <vector>
#include <Windows.h>

class Block;
class Bullet;

class Tank {
public:
    Tank();

    struct Position {
        int y1;
        int x1;
        int y2;
        int x2;
    };

    Position position;
    Position tempPosition;

    int blockPositionX;
    int blockPositionY;
    static const int Up = 0;
    static const int Right = 1;
    static const int Down = 2;
    static const int Left = 3;
    static const int WIDTH = 30;
    static const int HEIGHT = 32;

    void movement(Block Map[HEIGHT][WIDTH], std::vector<Tank>& tanks, HWND hwnd); // Добавлено: принимает вектор танков
    void Shoot(Block Map[HEIGHT][WIDTH]);
    bool damageThis(int damage);
    std::vector<Bullet>& GetBullets();
    void SetAngle(int angle);
    void setVectorMoving(int[4]);
    int GetAngle() const;
    int GetTempAngle() const;
    bool checkMove(Position pos, Block Map[HEIGHT][WIDTH], HWND hwnd);
    bool isColliding(Position pos1, Position pos2) const;
    int GetHealth() const;
    int GetTempHealth() const;
    void SetHealth(int newHealth);
    bool isAlive();
    void setDamage(int newDamage);
    void mainAI();

    int movingVector[4];
protected:
    std::vector<Bullet> bullets;
    int angle;
    int tempAngle;
    int speed;
    int health;
    int tempHealth;
    int damage;
    bool live;
};

