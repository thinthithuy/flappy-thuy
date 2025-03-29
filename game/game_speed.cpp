#include "game_speed.h"

GameSpeedManager::GameSpeedManager() {
    speedMultiplier = 1.0f; // Bắt đầu với tốc độ bình thường
    lastScoreCheckpoint = 0; // Ban đầu chưa có điểm số
}

void GameSpeedManager::updateSpeed(int score) {
    if (score >= lastScoreCheckpoint + scoreThreshold) {
        speedMultiplier += speedIncrement; // Tăng tốc độ
        lastScoreCheckpoint = score; // Cập nhật mốc điểm số
    }
}

float GameSpeedManager::getSpeedMultiplier() const {
    return speedMultiplier;
}
