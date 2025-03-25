#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <cstdlib> // Cho rand()
#include <SDL_ttf.h>
#include <fstream> // Đọc/ghi file
#include <cmath>

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const char* WINDOW_TITLE = "flappy thuy";

const int PIPE_WIDTH = 80;
const int PIPE_GAP = 150;
const int PIPE_SPEED = 3;

// Cấu trúc lưu thông tin ống nước
struct Pipe {
    int x, height;
    bool scored = false;
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
SDL_Texture* pipeTexture = nullptr;

void drawPipe(int x, int height, SDL_Renderer* renderer) {
    // Cột trên (lộn ngược)
    SDL_Rect topPipe = {x, 0, PIPE_WIDTH, height};
    SDL_Rect srcRect = {0, 0, 80, 400}; // Điều chỉnh theo ảnh thực tế
    SDL_RenderCopyEx(renderer, pipeTexture, &srcRect, &topPipe, 0, NULL, SDL_FLIP_VERTICAL);

    // Cột dưới
    SDL_Rect bottomPipe = {x, height + PIPE_GAP, PIPE_WIDTH, SCREEN_HEIGHT - height - PIPE_GAP};
    SDL_RenderCopy(renderer, pipeTexture, &srcRect, &bottomPipe);
}

// Thêm ống nước mới
void addPipe() {
    int height = rand() % (SCREEN_HEIGHT - PIPE_GAP - 100) + 50;
    pipes.push_back({SCREEN_WIDTH, height});
}

// Cập nhật vị trí của ống nước
void updatePipes()
{
    for (auto &pipe : pipes)
    {
        pipe.x -= PIPE_SPEED; // Di chuyển sang trái
    }

    // Xóa ống nước khi nó ra khỏi màn hình
    if (!pipes.empty() && pipes[0].x < -PIPE_WIDTH)
    {
        pipes.erase(pipes.begin());
    }

    // Thêm ống mới nếu cần
    if (pipes.empty() || pipes.back().x < SCREEN_WIDTH - 250)
    {
        addPipe();
    }
}

// Vẽ tất cả ống nước lên màn hình
void renderPipes(SDL_Renderer* renderer)
{
    for (auto &pipe : pipes)
    {
        drawPipe(pipe.x, pipe.height, renderer);
    }
}

struct Bird {
    float x, y;       // Vị trí của chim
    float velocity;   // Vận tốc rơi
    float gravity;    // Trọng lực
};

SDL_Texture* birdTexture = nullptr;
Bird bird = {100, SCREEN_HEIGHT / 2, 0, 0.5f}; // Vị trí ban đầu của chim

void drawBird(SDL_Renderer* renderer) {
    SDL_Rect dest = { (int)bird.x, (int)bird.y, 50, 50 }; // Kích thước chim
    SDL_RenderCopy(renderer, birdTexture, NULL, &dest);
}

void updateBird(bool gameStarted) {
    if (!gameStarted) {
        // Khi chưa bắt đầu, cho chim dao động nhẹ (lên xuống)
        static float time = 0;
        bird.y = (SCREEN_HEIGHT / 2) + sin(time) * 10; // Dao động quanh vị trí ban đầu
        time += 0.1f;
    } else {
        // Khi game đã bắt đầu, chim rơi tự nhiên
        bird.velocity += bird.gravity;
        bird.y += bird.velocity;

        // Giới hạn chim trong màn hình
        if (bird.y < 0) bird.y = 0;
        if (bird.y > SCREEN_HEIGHT - 50) bird.y = SCREEN_HEIGHT - 50;
    }
}


bool checkCollision(const Bird& bird, const Pipe& pipe) {
    SDL_Rect birdRect = {(int)bird.x, (int)bird.y, 50, 50}; // Hình chữ nhật của chim
    SDL_Rect topPipeRect = {pipe.x, 0, PIPE_WIDTH, pipe.height}; // Ống trên
    SDL_Rect bottomPipeRect = {pipe.x, pipe.height + PIPE_GAP, PIPE_WIDTH, SCREEN_HEIGHT - pipe.height - PIPE_GAP}; // Ống dưới

    // Kiểm tra va chạm với ống trên
    if (SDL_HasIntersection(&birdRect, &topPipeRect)) {
        return true;
    }

    // Kiểm tra va chạm với ống dưới
    if (SDL_HasIntersection(&birdRect, &bottomPipeRect)) {
        return true;
    }

    // Kiểm tra nếu chim chạm đất
    if (bird.y >= SCREEN_HEIGHT - 50) { // 50 là chiều cao chim
        return true;
    }

    return false;
}

SDL_Texture* gameOverTexture = nullptr;
void drawGameOver(SDL_Renderer* renderer) {
    SDL_Rect dest = { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, 200, 100 }; // Điều chỉnh kích thước
    SDL_RenderCopy(renderer, gameOverTexture, NULL, &dest);
}

int score = 0;
int highScore = 0;
TTF_Font* font = nullptr;
SDL_Color whiteColor = {255, 255, 255};

void loadHighScore() {
    std::ifstream file("highscore.txt");
    if (file.is_open()) {
        file >> highScore;
        file.close();
    }
}

void saveHighScore() {
    std::ofstream file("highscore.txt");
    if (file.is_open()) {
        file << highScore;
        file.close();
    }
}

void renderText(SDL_Renderer* renderer, const std::string& text, int x, int y) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), whiteColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dest = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dest);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void updateScore(bool gameOver) {
    for (auto &pipe : pipes) {
        if (!gameOver && pipe.x + PIPE_WIDTH < bird.x && pipe.scored == false) {
            score++;
            pipe.scored = true;
        }
    }
}

int main(int argc, char* argv[]) {
    // Khởi tạo SDL, cửa sổ, renderer
    SDL_Window* window = initSDL(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SDL_Renderer* renderer = createRenderer(window);

    // Tải hình ảnh
    SDL_Texture* background = loadTexture("backgr.jpg", renderer);
    pipeTexture = loadTexture("pipe.png", renderer);
    birdTexture = loadTexture("bird.png", renderer);
    gameOverTexture = loadTexture("gameover.png", renderer);

    // Khởi tạo font chữ để hiển thị điểm số
    TTF_Init();
    font = TTF_OpenFont("arial.ttf", 28);
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        return -1;
    }

    // Đọc điểm cao nhất từ file
    loadHighScore();

    // Trạng thái trò chơi
    bool running = true;
    bool gameOver = false;
    bool gameStarted = false; // Trò chơi chưa bắt đầu
    SDL_Event event;

    while (running) {
        // Xử lý sự kiện
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_SPACE) {
                    if (!gameStarted) {
                        gameStarted = true;  // Bắt đầu trò chơi khi nhấn SPACE lần đầu
                        addPipe(); // Thêm ống nước đầu tiên
                    }
                    if (!gameOver) {
                        bird.velocity = -8; // Chim nhảy lên
                    } else {
    // Nếu game over, nhấn SPACE để chơi lại
    gameOver = false;
    gameStarted = false;
    score = 0;
    bird = {100, SCREEN_HEIGHT / 2, 0, 0.5f}; // Reset vị trí chim
    pipes.clear(); // Xóa toàn bộ ống nước
}
                }
            }
        }

        if (!gameOver) {
            updateBird(gameStarted);  // ✅ Chim vẫn bay trước khi bắt đầu game

            if (gameStarted) {
                updatePipes(); // Ống chỉ xuất hiện sau khi bắt đầu
                updateScore(gameOver);

                // Kiểm tra va chạm
                for (auto &pipe : pipes) {
                    if (checkCollision(bird, pipe)) {
                        gameOver = true;
                    }
                }
            }
        }

        // Vẽ các thành phần
        SDL_RenderCopy(renderer, background, NULL, NULL);
        renderPipes(renderer);
        drawBird(renderer);

        // Hiển thị điểm số
        renderText(renderer, "Score: " + std::to_string(score), 20, 20);
        renderText(renderer, "High Score: " + std::to_string(highScore), 20, 50);

        // Nếu chưa bắt đầu, hiển thị hướng dẫn
        if (!gameStarted) {
            renderText(renderer, "Press SPACE to start!", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50);
        }

        if (gameOver) {
            drawGameOver(renderer);
            if (score > highScore) {
                highScore = score;
                saveHighScore();
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(20);
    }

    // Giải phóng tài nguyên
    SDL_DestroyTexture(gameOverTexture);
    SDL_DestroyTexture(birdTexture);
    SDL_DestroyTexture(pipeTexture);
    SDL_DestroyTexture(background);
    TTF_CloseFont(font);
    TTF_Quit();
    quitSDL(window, renderer);

    return 0;
}
