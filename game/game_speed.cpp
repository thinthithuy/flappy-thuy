#include "game_speed.h"
#include <iostream>

// Constructor
GameSpeed::GameSpeed() : speed(3.0f), columnCount(0) {}

// Cập nhật tốc độ nếu đã vượt qua 15 cột
void GameSpeed::updateSpeed() {
    if (columnCount > 0 && columnCount % COLUMNS_PER_SPEED_INCREASE == 0) {
        speed += SPEED_INCREMENT;
        std::cout << "Tốc độ hiện tại: " << speed << std::endl;
    }
}

// Trả về tốc độ hiện tại
float GameSpeed::getSpeed() const {
    return speed;
}

// Tăng số lượng cột đã vượt qua
void GameSpeed::incrementColumn() {
    columnCount++;
    updateSpeed();
}

// Reset tốc độ khi chơi lại
void GameSpeed::reset() {
    speed = 3.0f;
    columnCount = 0;
}

