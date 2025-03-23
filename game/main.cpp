#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <cstdlib> // Cho rand()

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const char* WINDOW_TITLE = "flappy thuy";

const int PIPE_WIDTH = 80;
const int PIPE_GAP = 150;
const int PIPE_SPEED = 4;

// Cấu trúc lưu thông tin ống nước
struct Pipe {
    int x, height;
};

// Danh sách ống nước
vector<Pipe> pipes;

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
    while (true) {
        if ( SDL_PollEvent(&e) != 0 &&
             (e.type == SDL_KEYDOWN || e.type == SDL_QUIT) )
            return;
        SDL_Delay(100);
    }
}

void renderTexture(SDL_Texture *texture, int x, int y, SDL_Renderer* renderer)
{
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
    SDL_RenderCopy(renderer, texture, NULL, &dest);
}

SDL_Texture *loadTexture(const char *filename, SDL_Renderer* renderer)
{
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);

    SDL_Texture *texture = IMG_LoadTexture(renderer, filename);
    if (texture == NULL)
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Load texture %s", IMG_GetError());

    return texture;
}

// Hàm vẽ ống nước bằng hình chữ nhật màu xanh lá cây
void drawPipe(int x, int height, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Màu xanh lá cây

    // Cột trên
    SDL_Rect topPipe = {x, 0, PIPE_WIDTH, height};
    SDL_RenderFillRect(renderer, &topPipe);

    // Cột dưới
    SDL_Rect bottomPipe = {x, height + PIPE_GAP, PIPE_WIDTH, SCREEN_HEIGHT - height - PIPE_GAP};
    SDL_RenderFillRect(renderer, &bottomPipe);
}

// Thêm ống nước mới
void addPipe() {
    int height = rand() % (SCREEN_HEIGHT - PIPE_GAP - 100) + 50;
    pipes.push_back({SCREEN_WIDTH, height});
}

// Cập nhật vị trí của ống nước
void updatePipes() {
    for (auto &pipe : pipes) {
        pipe.x -= PIPE_SPEED; // Di chuyển sang trái
    }

    // Xóa ống nước khi nó ra khỏi màn hình
    if (!pipes.empty() && pipes[0].x < -PIPE_WIDTH) {
        pipes.erase(pipes.begin());
    }

    // Thêm ống mới nếu cần
    if (pipes.empty() || pipes.back().x < SCREEN_WIDTH - 250) {
        addPipe();
    }
}

// Vẽ tất cả ống nước lên màn hình
void renderPipes(SDL_Renderer* renderer) {
    for (auto &pipe : pipes) {
        drawPipe(pipe.x, pipe.height, renderer);
    }
}

int main(int argc, char* argv[])
{
    SDL_Window* window = initSDL(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SDL_Renderer* renderer = createRenderer(window);

    SDL_Texture* background = loadTexture("backgr.jpg", renderer);

    // Thêm ống nước ban đầu
    addPipe();

    bool running = true;
    SDL_Event event;

    while (running) {
        // Xử lý sự kiện
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Cập nhật ống nước
        updatePipes();

        // Vẽ màn hình nền
        SDL_RenderCopy(renderer, background, NULL, NULL);

        // Vẽ ống nước
        renderPipes(renderer);

        SDL_RenderPresent(renderer);
        SDL_Delay(20);
    }

    SDL_DestroyTexture(background);
    quitSDL(window, renderer);

    return 0;
}
//hihi
