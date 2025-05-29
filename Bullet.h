#pragma once
#include <Windows.h>

class Block; // Forward declaration
class Tank;
#include <vector>

class Bullet {
public:
    Bullet(int x, int y, int angle, int speed);
    ~Bullet();

    // Добавляем move-семантику
    Bullet(Bullet&& other) noexcept;
    Bullet& operator=(Bullet&& other) noexcept;

    static const int WIDTH = 30;
    static const int HEIGHT = 32;

    void Move(Block Map[HEIGHT][WIDTH], HWND hwnd, std::vector<Tank>& tanks); // Изменено: принимает вектор танков
    bool IsAlive();
    void Draw(HDC hdc);
    RECT GetRect() const;
    void togleIsAlive(bool alive);
    bool isColliding(RECT rect1, RECT rect2) const; // Функция для проверки столкновений

private:

    static const int maxLifetime = 10000;
    static const int BULLET_SIZE = 4;

    int x;
    int y;
    int tempX;
    int tempY;

    int angle;
    int speed;
    int lifetime;
    bool isAlive;
};