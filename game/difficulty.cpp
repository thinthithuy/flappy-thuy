#include "Difficulty.h"

// Khởi tạo giá trị mặc định
double Difficulty::pipeSpeed = 2;

void Difficulty::updateDifficulty(int score) {
    if (score >= 10) {
        pipeSpeed = 2.0000000000000001;
    }
    if (score >= 30) {
        pipeSpeed = 2.0000000000000002;
    }
    if (score >= 50) {
        pipeSpeed = 2.0000000000000003;
    }
}
