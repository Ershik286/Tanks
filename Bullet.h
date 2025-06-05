#pragma once
#include <Windows.h>
#include <string>
#include <vector>

class Block;
class Tank;

class Bullet {
public:
    Bullet(int x, int y, int angle, int speed);
    ~Bullet();

    // Удаляем конструктор копирования
    Bullet(const Bullet&) = delete;
    Bullet& operator=(const Bullet&) = delete;

    // Move-семантика
    Bullet(Bullet&& other) noexcept;
    Bullet& operator=(Bullet&& other) noexcept;

    static const int WIDTH = 30;
    static const int HEIGHT = 32;

    void Move(Block Map[HEIGHT][WIDTH], HWND hwnd, std::vector<Tank>& tanks, int numberTanks);
    bool IsAlive();
    void Draw(HDC hdc);
    RECT GetRect() const;
    void togleIsAlive(bool alive);
    bool isColliding(RECT rect1, RECT rect2) const;
    void clearDraw(HDC hdc);

    int x;
    int y;
    int tempX;
    int tempY;

private:
    static const int maxLifetime = 10000;
    static const int BULLET_SIZE = 4;

    int angle;
    int speed;
    int lifetime;
    bool isAlive;
};
