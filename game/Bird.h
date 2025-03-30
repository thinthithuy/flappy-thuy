#ifndef BIRD.H
#define BIRD.H
#include <SDL.h>
#include <SDL_image.h>
#include <cmath>
class Bird
{
private:
    SDL_Rect dst;
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
    Bird(float sx,float sy,float vel,float grav);
    void reset(float sx,float sy,float vel,float grav);
    void drawBird(SDL_Renderer* renderer,SDL_Texture* texture);
    void updateBird(bool gameStarted);
};
#endif // BIRD

