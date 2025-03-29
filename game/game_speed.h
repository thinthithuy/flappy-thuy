#ifndef GAME_SPEED_H
#define GAME_SPEED_H

class GameSpeedManager {
private:
    float speedMultiplier; // Hệ số tốc độ
    int lastScoreCheckpoint; // Mốc điểm số để tăng tốc độ
    const float speedIncrement = 0.1f; // Mức tăng mỗi lần đạt điểm
    const int scoreThreshold = 5; // Số điểm cần để tăng tốc

public:
    GameSpeedManager(); // Constructor

    void updateSpeed(int score); // Cập nhật tốc độ dựa trên điểm số
    float getSpeedMultiplier() const; // Lấy giá trị tốc độ hiện tại
};

#endif // GAME_SPEED_H
