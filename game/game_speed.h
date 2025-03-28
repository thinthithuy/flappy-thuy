#ifndef GAME_SPEED_H
#define GAME_SPEED_H

class GameSpeed {
private:
    float speed;  // Tốc độ hiện tại
    int columnCount; // Số cột đã vượt qua
    const int COLUMNS_PER_SPEED_INCREASE = 1; // Số cột để tăng tốc
    const float SPEED_INCREMENT = 0.5f; // Mức tăng tốc độ mỗi 15 cột

public:
    GameSpeed();  // Constructor
    void updateSpeed(); // Cập nhật tốc độ
    float getSpeed() const; // Lấy tốc độ hiện tại
    void incrementColumn(); // Tăng số lượng cột đã vượt qua
    void reset(); // Reset tốc độ khi chơi lại
};

#endif // GAME_SPEED_H

