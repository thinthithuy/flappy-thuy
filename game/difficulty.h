#ifndef DIFFICULTY_H
#define DIFFICULTY_H

class Difficulty {
public:
    static double pipeSpeed;  // Tốc độ di chuyển của ống

    static void updateDifficulty(int score); // Cập nhật độ khó dựa vào điểm số
};

#endif
