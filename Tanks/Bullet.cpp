#include "Bullet.h"
#include "Block.h"
#include "Tank.h"
#include <cmath>
#include <Windows.h>
#include <iostream>
#include <WinUser.h>
#pragma comment(lib, "User32.lib")

#define M_PI 3.14

Bullet::Bullet(int x, int y, int angle, int speed, Tank* thisTanks)
    : x(x), y(y), angle(angle), speed(speed), lifetime(10000), isAlive(true),
    tempX(x), tempY(y), damage(thisTanks->GetDamage()){ // Сохраняем указатель на оригинальный танк
}

Bullet::~Bullet() {}

Bullet::Bullet(Bullet&& other) noexcept
    : x(other.x),
    y(other.y),
    angle(other.angle),
    speed(other.speed),
    lifetime(other.lifetime),
    isAlive(other.isAlive),
    tempX(other.tempX),
    tempY(other.tempY)
{
}

Bullet& Bullet::operator=(Bullet&& other) noexcept
{
    if (this != &other) {
        x = other.x;
        y = other.y;
        tempX = other.tempX;
        tempY = other.tempY;
        angle = other.angle;
        speed = other.speed;
        lifetime = other.lifetime;
        isAlive = other.isAlive;
    }
    return *this;  // Добавлен возврат значения
}

void Bullet::Move(Block Map[HEIGHT][WIDTH], HWND hwnd, std::vector<Tank>& tanks, int numberTanks) {
    if (!isAlive) return;

    // Сохраняем предыдущую позицию
    this->tempX = x;
    this->tempY = y;

    //if (parents == nullptr) {
    //    isAlive = false;
    //    MessageBoxW(hwnd, L"БАн бын бубын", L"Ошибка", NULL);
    //    return;
    //}

    // Движение пули
    double angleRad = angle * M_PI / 180.0;
    this->x += static_cast<int>(speed * cos(angleRad));
    this->y -= static_cast<int>(speed * sin(angleRad));

    // Проверка выхода за границы карты
    if (x < 0 || x >= WIDTH * 16 || y < 0 || y >= HEIGHT * 16) {
        this->isAlive = false;
        return;
    }

    // Проверка столкновений с блоками
    RECT bulletRect = GetRect();
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (Map[i][j].getNumberBlock() != Block::emptiness &&
                Map[i][j].getNumberBlock() != Block::foliage) {
                RECT blockRect = {
                    j * 16,
                    i * 16,
                    (j + 1) * 16,
                    (i + 1) * 16
                };
                if (isColliding(bulletRect, blockRect)) {
                    Map[i][j].damage(damage, hwnd);
                    this->isAlive = false;
                    return;
                }
            }
        }
    }

    int countTank = 0;
    // Проверка столкновений с танками
    for (Tank& tank : tanks) {
        if (tank.GetHealth() <= 0) continue;

        RECT tankRect = { tank.position.x1, tank.position.y1, tank.position.x2, tank.position.y2 };
        if (isColliding(bulletRect, tankRect)) {
            bool isDestroyed = tank.damageThis(damage);
            this->isAlive = false;
            return;
        }
        countTank++;
    }

    this->lifetime--;
    if (lifetime <= 0) {
        this->isAlive = false;
    }
}

bool Bullet::IsAlive() {
    return isAlive;
}

void Bullet::Draw(HDC hdc) {
    // Очищаем предыдущую позицию тем же цветом, что и поле
    RECT clearRect = {
        tempX - BULLET_SIZE / 2,
        tempY - BULLET_SIZE / 2,
        tempX + BULLET_SIZE / 2,
        tempY + BULLET_SIZE / 2
    };
    HBRUSH clearingBrush = CreateSolidBrush(RGB(50, 50, 50)); // Используем цвет поля
    FillRect(hdc, &clearRect, clearingBrush);
    DeleteObject(clearingBrush);

    // Рисуем новую позицию
    if (isAlive) {
        HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
        Ellipse(hdc,
            x - BULLET_SIZE / 2,
            y - BULLET_SIZE / 2,
            x + BULLET_SIZE / 2,
            y + BULLET_SIZE / 2);
        SelectObject(hdc, oldBrush);
        DeleteObject(brush);
    }
}

RECT Bullet::GetRect() const {
    RECT bulletRect;
    bulletRect.left = x - BULLET_SIZE;
    bulletRect.top = y - BULLET_SIZE;
    bulletRect.right = x + BULLET_SIZE;
    bulletRect.bottom = y + BULLET_SIZE;
    return bulletRect;
}

void Bullet::togleIsAlive(bool alive) {
    this->isAlive = alive;
}

bool Bullet::isColliding(RECT rect1, RECT rect2) const {
    return !(rect1.right < rect2.left || rect1.left > rect2.right || rect1.bottom < rect2.top || rect1.top > rect2.bottom);
}

void Bullet::clearDraw(HDC hdc) {
    RECT clearRect = {
    tempX - BULLET_SIZE / 2,
    tempY - BULLET_SIZE / 2,
    tempX + BULLET_SIZE / 2,
    tempY + BULLET_SIZE / 2
    };
    HBRUSH clearingBrush = CreateSolidBrush(RGB(255, 255, 255));
    FillRect(hdc, &clearRect, clearingBrush);
    DeleteObject(clearingBrush);
}
