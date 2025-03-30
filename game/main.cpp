#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <SDL_ttf.h>
#include <fstream>
#include "audiomanager.h"
#include "Bird.h"
#include "background.h"
#include <cmath>
using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const char* WINDOW_TITLE = "flappy thuy";


Background background;
// Cấu trúc lưu thông tin ống nước

// Danh sách ống nước

void logErrorAndExit(const char* msg, const char* error)
{
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "%s: %s", msg, error);
    SDL_Quit();
}

SDL_Window* initSDL(int SCREEN_WIDTH, int SCREEN_HEIGHT, const char* WINDOW_TITLE)
{
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
        logErrorAndExit("SDL_Init", SDL_GetError());

    SDL_Window* window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (window == nullptr) logErrorAndExit("CreateWindow", SDL_GetError());

    if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))
        logErrorAndExit( "SDL_image error:", IMG_GetError());

    return window;
}

SDL_Renderer* createRenderer(SDL_Window* window)
{
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    return renderer;
}

void quitSDL (SDL_Window* window, SDL_Renderer* renderer)
{
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void waitUntilKeyPressed()
{
    SDL_Event e;
    while (true)
    {
        if ( SDL_PollEvent(&e) != 0 &&
           (e.type == SDL_KEYDOWN || e.type == SDL_QUIT) )
            return;
        SDL_Delay(100);
    }
}

Bird bird(100, SCREEN_HEIGHT / 2, 0, 0.5f); // Vị trí ban đầu của chim

int score = 0;
int highScore = 0;
TTF_Font* font = nullptr;
SDL_Color whiteColor = {255, 255, 255};

void loadHighScore()
{
    ifstream file("highscore.txt");
    if (file.is_open())
    {
        file >> highScore;
        file.close();
    }
}

void saveHighScore()
{
    ofstream file("highscore.txt");
    if (file.is_open())
    {
        file << highScore;
        file.close();
    }
}

void renderText(SDL_Renderer* renderer, const string& text, int x, int y)
{
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), whiteColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dest = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dest);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void updateScore(bool gameOver)
{
    score+= background.updateScore(gameOver,bird);
}

int main(int argc, char* argv[])
{
    // Khởi tạo SDL, cửa sổ, renderer
    SDL_Window* window = initSDL(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SDL_Renderer* renderer = createRenderer(window);
    SDL_Init(SDL_INIT_EVERYTHING);

     // Khởi tạo hệ thống âm thanh
    AudioManager& audio = AudioManager::getInstance();
    if (!audio.init())
    {
        return -1;
    }

    // Load âm thanh
    audio.loadMusic("background", "background.mp3");
    audio.loadSound("flap", "flap.mp3");
    audio.loadSound("hit", "hit.mp3");

    background.Init_Background(renderer);
    // Phát nhạc nền
    audio.playMusic("background");

    TTF_Init();
    font = TTF_OpenFont("arial.ttf", 28);
    if (!font)
    {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        return -1;
    }

    loadHighScore();

    // Trạng thái trò chơi
    bool running = true;
    bool gameOver = false;
    bool gameStarted = false; // Trò chơi chưa bắt đầu
    SDL_Event event;

    // Trạng thái tạm dừng
    bool paused = false;

    while (running)
    {
        // Xử lý sự kiện
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_SPACE)
                {
                    if (!gameStarted)
                    {
                        gameStarted = true;  // Bắt đầu trò chơi khi nhấn SPACE lần đầu
                        background.addPipe(); // Thêm ống nước đầu tiên
                    }
                    if (!gameOver)
                    {
                        bird.update_velociity();
                        audio.playSound("flap");
                    }
                    else
                    {
                        // Nếu game over, nhấn SPACE để chơi lại
                        gameOver = false;
                        gameStarted = false;
                        score = 0;
                        background.reset();

                        bird.reset(100, SCREEN_HEIGHT / 2, 0, 0.5f); // Reset vị trí chim
                    }
                }
                // Thoát game khi nhấn ENTER
                else if (event.key.keysym.sym == SDLK_RETURN)
                {
                    running = false;
                }
                // Tạm dừng khi nhấn P
                else if (event.key.keysym.sym == SDLK_p)
                {
                    paused = !paused;
                }
            }
        }
        if (!gameOver && !paused)
        {
             // Chỉ cập nhật khi không game over và không tạm dừng
            bird.updateBird(gameStarted);

            if (gameStarted)
            {
                background.updatePipes(); // Ống chỉ xuất hiện sau khi bắt đầu
                updateScore(gameOver);

                // Kiểm tra va chạm
                background.check_coll(bird,audio,gameOver);
            }
        }

        // Vẽ các thành phần
        SDL_RenderClear(renderer);
        background.render_background(renderer,bird);
        // Hiển thị điểm số
        renderText(renderer, "Score: " + to_string(score), 20, 20);
        renderText(renderer, "High Score: " + to_string(highScore), 20, 50);

        // Nếu chưa bắt đầu, hiển thị hướng dẫn
        if (!gameStarted)
        {
            renderText(renderer, "Press SPACE to start!", SCREEN_WIDTH / 2 - 125, SCREEN_HEIGHT / 2 - 50);
        }

        if (paused)
        {
            renderText(renderer, "PAUSED - Press P to Resume", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2);
        }

        if (gameOver)
        {
            background.drawGameOver(renderer);
            if (score > highScore)
            {
                highScore = score;
                saveHighScore();
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(20);
    }

    // Giải phóng tài nguyên

    TTF_CloseFont(font);
    TTF_Quit();
    audio.close();
    quitSDL(window, renderer);

    return 0;
}
