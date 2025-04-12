#ifndef BIRD.H
#define BIRD.H
#include <SDL.h>
#include <SDL_image.h>
#include <cmath>
class Bird
{
private:
    SDL_Rect dst; //Hình chữ nhật xác định nơi chim được vẽ ra màn hình
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;
    float x, y;       // Vị trí của chim
    float velocity;   // Vận tốc rơi
    float gravity;    // Trọng lực
public:
    float Get_x()
    {
        return x;
    }
    float Get_y()
    {
        return y;
    }
    void update_velociity()
    {
        velocity = -8;
    }
    Bird(float sx,float sy,float vel,float grav);//khởi tạo chim với vị trí ban đầu sx,sy, vận tốc ban đầu và trọng lực
    void reset(float sx,float sy,float vel,float grav);//Dùng để đặt lại trạng thái chim về ban đầu khi bắt đầu lại game
    void drawBird(SDL_Renderer* renderer,SDL_Texture* texture);//Dùng để vẽ chim lên màn hình tại tọa độ (x, y), sử dụng texture là hình ảnh chim (bird.png).
    void updateBird(bool gameStarted);//cập nhật vị trí chim theo thời gian
    //Nếu gameStarted == true, thì chim bắt đầu rơi dần xuống theo trọng lực
};
#endif

