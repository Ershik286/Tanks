#include "Bullet.h"
#include "Block.h"
#include "Tank.h"
#include <cmath>

#define M_PI 3.14

Bullet::Bullet(int x, int y, int angle, int speed)
    : x(x), y(y), angle(angle), speed(speed), lifetime(10000), isAlive(true) {
    this->tempX = x;
    this->tempY = y;
}

Bullet::~Bullet() {}

// Реализация move-конструктора
Bullet::Bullet(Bullet&& other) noexcept
    : x(other.x), y(other.y), angle(other.angle), speed(other.speed),
    lifetime(other.lifetime), isAlive(other.isAlive) {
}

// Реализация move-оператора присваивания
Bullet& Bullet::operator=(Bullet&& other) noexcept {
    if (this != &other) {
        x = other.x;
        y = other.y;
        angle = other.angle;
        speed = other.speed;
        lifetime = other.lifetime;
        isAlive = other.isAlive;
    }
    return *this;
}

void Bullet::Move(Block Map[HEIGHT][WIDTH], HWND hwnd, std::vector<Tank>& tanks) {
    if (!isAlive) return;

    this->tempX = x;
    this->tempY = y;

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
                    Map[i][j].damage(10, hwnd);
                    this->isAlive = false;
                    return;
                }
            }
        }
    }

    // Проверка столкновений с танками
    for (Tank& tank : tanks) {
        if (tank.GetHealth() <= 0) continue;

        RECT tankRect = { tank.position.x1, tank.position.y1, tank.position.x2, tank.position.y2 };
        if (isColliding(bulletRect, tankRect)) {
            bool isDestroyed = tank.damageThis(10);
            this->isAlive = false;
            if (isDestroyed) {
                MessageBox(hwnd, L"Танк уничтожен!", L"Сообщение", MB_OK);
            }
            return;
        }
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
    // Очистка предыдущей позиции всегда, если пуля мертва или переместилась
    if (!isAlive || tempX != x || tempY != y) {
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

    // Рисование пули только если она жива
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

void Bullet::togleIsAlive(bool alive){
    this->isAlive = alive;
}

bool Bullet::isColliding(RECT rect1, RECT rect2) const {
    return !(rect1.right < rect2.left || rect1.left > rect2.right || rect1.bottom < rect2.top || rect1.top > rect2.bottom);
}