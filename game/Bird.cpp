#include "Bird.h"

Bird::Bird(float sx,float sy,float vel,float grav)
{
    x = sx;
    y = sy;
    velocity = vel;
    gravity = grav;
}
void Bird::reset(float sx,float sy,float vel,float grav)
{
    x = sx;
    y = sy;
    velocity = vel;
    gravity = grav;
}

void Bird::drawBird(SDL_Renderer* renderer,SDL_Texture* texture)
{
    dst = {(int)x, (int)y, 50, 50 }; // Kích thước chim
    SDL_RenderCopy(renderer, texture, NULL, &dst);
}
void Bird::updateBird(bool gameStarted)
{
    if (!gameStarted)
    {
        // Khi chưa bắt đầu, cho chim dao động nhẹ (lên xuống)
        static float time = 0; // giữ lại giá trị của biến giữa các lần gọi hàm, thay vì khởi tạo lại từ đầu mỗi khi hàm được gọi.
        y = (SCREEN_HEIGHT / 2) + sin(time) * 10; // Dao động quanh vị trí ban đầu
        time += 0.1f;//tăng thời gian để sin() thay đổi → tạo hiệu ứng động.
    }
    else
    {
        // Khi game đã bắt đầu, chim rơi tự nhiên
        velocity += gravity;
        y += velocity;

        // Giới hạn chim trong màn hình
        if (y < 0) y = 0;
        if (y > SCREEN_HEIGHT - 50) y = SCREEN_HEIGHT - 50;
    }
}

