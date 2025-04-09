#include "background.h"
#include "Bird.h"
using namespace std;


void Background::drawGameOver(SDL_Renderer* renderer)
{
    SDL_Rect dest = { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, 200, 100 }; // Điều chỉnh kích thước
    SDL_RenderCopy(renderer, gameOverTexture, NULL, &dest);
}
void Background::Init_Background(SDL_Renderer* renderer)
{

    pipeTexture = loadTexture("pipe.png", renderer);
    birdTexture = loadTexture("bird.png", renderer);
    background = loadTexture("backgr.jpg", renderer);
    gameOverTexture = loadTexture("gameover.png", renderer);
}
void Background::renderTexture(SDL_Texture *texture, int x, int y, SDL_Renderer* renderer)
{
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
    SDL_RenderCopy(renderer, texture, NULL, &dest);
}
SDL_Texture* Background::loadTexture(const char *filename, SDL_Renderer* renderer)
{
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);

    SDL_Texture *texture = IMG_LoadTexture(renderer, filename);
    if (texture == NULL)
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Load texture %s", IMG_GetError());

    return texture;
}

void Background::drawPipe(int x, int height, SDL_Renderer* renderer)
{
    // Cột trên (lộn ngược)
    SDL_Rect topPipe = {x, 0, PIPE_WIDTH, height};
    SDL_Rect srcRect = {0, 0, 80, 400}; // Điều chỉnh theo ảnh thực tế
    SDL_RenderCopyEx(renderer, pipeTexture, &srcRect, &topPipe, 0, NULL, SDL_FLIP_VERTICAL);

    // Cột dưới
    SDL_Rect bottomPipe = {x, height + PIPE_GAP, PIPE_WIDTH, SCREEN_HEIGHT - height - PIPE_GAP};
    SDL_RenderCopy(renderer, pipeTexture, &srcRect, &bottomPipe);
}

void Background::addPipe()
{
    int height = rand() % (SCREEN_HEIGHT - PIPE_GAP - 100) + 50;
    pipes.push_back({SCREEN_WIDTH, height});
}

void Background::updatePipes()
{
    for (auto &pipe : pipes)
    {
        pipe.x -= PIPE_SPEED;
    }

    if (!pipes.empty() && pipes[0].x < -PIPE_WIDTH)
    {
        pipes.erase(pipes.begin());
    }

    if (pipes.empty() || pipes.back().x < SCREEN_WIDTH - 250)
    {
        addPipe();
    }
}

void Background::renderPipes(SDL_Renderer* renderer)
{
    for (auto &pipe : pipes)
    {
        drawPipe(pipe.x, pipe.height, renderer);
    }
}
bool Background::checkCollision(Bird& bird, const Pipe& pipe)
{
    SDL_Rect birdRect = {(int)bird.Get_x(), (int)bird.Get_y(), 50, 50}; // Hình chữ nhật của chim
    SDL_Rect topPipeRect = {pipe.x, 0, PIPE_WIDTH, pipe.height}; // Ống trên
    SDL_Rect bottomPipeRect = {pipe.x, pipe.height + PIPE_GAP, PIPE_WIDTH, SCREEN_HEIGHT - pipe.height - PIPE_GAP}; // Ống dưới

    // Kiểm tra va chạm với ống trên
    if (SDL_HasIntersection(&birdRect, &topPipeRect))
    {
        return true;
    }

    // Kiểm tra va chạm với ống dưới
    if (SDL_HasIntersection(&birdRect, &bottomPipeRect))
    {
        return true;
    }

    // Kiểm tra nếu chim chạm đất
    if (bird.Get_y() >= SCREEN_HEIGHT - 50)
    { // 50 là chiều cao chim
        return true;
    }

    return false;
}
void Background::render_background(SDL_Renderer* render,Bird& bird)
{
    //SDL_Rect drs = {SCREEN_WIDTH/2,SCREEN_HEIGHT/2,SCREEN_WIDTH,SCREEN_HEIGHT};
    SDL_RenderCopy(render, background, NULL, NULL);
    renderPipes(render);
    bird.drawBird(render,birdTexture);
}
void Background::check_coll(Bird& bird,AudioManager& audio,bool& gameOver)
{
    for (auto &pipe : pipes)
    {
        if (checkCollision(bird, pipe))
        {
            audio.playSound("hit");
            gameOver = true;
        }
    }
}

