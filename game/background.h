#ifndef BACKGROUND.H
#define BACKGROUND.H

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include "Bird.h"
#include "audiomanager.h"

using namespace std;

class Background
{
private:
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;
    const int PIPE_WIDTH = 80;
    const int PIPE_GAP = 150;
    const int PIPE_SPEED = 3;
    SDL_Texture* pipeTexture = nullptr;
    SDL_Texture* birdTexture = nullptr;
    SDL_Texture* background = nullptr;
    SDL_Texture* gameOverTexture = nullptr;

    struct Pipe
    {
        int x, height;
        bool scored = false;
    };

public:

    void reset()
    {
       pipes.clear();
    }

    bool checkCollision(Bird& bird, const Pipe& pipe);
    void Init_Background(SDL_Renderer* renderer);
    vector<Pipe> pipes;
    void renderTexture(SDL_Texture* texture, int x,int y,SDL_Renderer* render);
    SDL_Texture* loadTexture(const char *filename, SDL_Renderer* rernder);
    void drawPipe(int x, int height, SDL_Renderer* renderer);
    void addPipe();
    void updatePipes();
    void renderPipes(SDL_Renderer* renderer);
    void drawGameOver(SDL_Renderer* renderer);
    int updateScore(bool gameOver,Bird& bird)
    {
        int score = 0;
        for (auto &pipe : pipes)
        {
            if (!gameOver && pipe.x + PIPE_WIDTH < bird.Get_x() && pipe.scored == false)
            {
                score++;
                pipe.scored = true;
            }
        }
        return score;
    }
    void render_background(SDL_Renderer* render,Bird& bird);
    void check_coll(Bird& bird,AudioManager& audio,bool& gameOver);
    void clear_texture()
    {
        SDL_DestroyTexture(gameOverTexture);
        SDL_DestroyTexture(birdTexture);
        SDL_DestroyTexture(pipeTexture);
        SDL_DestroyTexture(background);
    }
};

#endif

