#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <cstdlib> // Cho rand()
#include <SDL_ttf.h> //thư viện hỗ trợ hiển thị văn bản


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

void updateBird() {
    bird.velocity += bird.gravity;  // Tăng tốc rơi
    bird.y += bird.velocity;        // Cập nhật vị trí chim

    // Giới hạn chim trong màn hình
    if (bird.y < 0) bird.y = 0;
    if (bird.y > SCREEN_HEIGHT - 50) bird.y = SCREEN_HEIGHT - 50;
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
TTF_Font* font = nullptr;
SDL_Color textColor = {255, 255, 255}; // Màu trắng
SDL_Texture* scoreTexture = nullptr;

// Khởi tạo SDL_ttf
void initFont() {
    if (TTF_Init() == -1) {
        logErrorAndExit("TTF_Init", TTF_GetError());
    }
    font = TTF_OpenFont("arial.ttf", 28); // Đổi thành font bạn có
    if (!font) {
        logErrorAndExit("TTF_OpenFont", TTF_GetError());
    }
}

// Tạo texture từ điểm số
SDL_Texture* renderText(const string& text, SDL_Renderer* renderer) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

// Hiển thị điểm số lên màn hình
void drawScore(SDL_Renderer* renderer) {
    if (scoreTexture) SDL_DestroyTexture(scoreTexture);
    scoreTexture = renderText("Score: " + to_string(score), renderer);

    SDL_Rect dest = {20, 20, 100, 50}; // Vị trí điểm số
    SDL_RenderCopy(renderer, scoreTexture, NULL, &dest);
}

int main(int argc, char* argv[]) {
    SDL_Window* window = initSDL(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SDL_Renderer* renderer = createRenderer(window);
    initFont(); // Khởi tạo font chữ

    SDL_Texture* background = loadTexture("backgr.jpg", renderer);
    pipeTexture = loadTexture("pipe.png", renderer);
    birdTexture = loadTexture("bird.png", renderer);
    gameOverTexture = loadTexture("gameover.png", renderer);

    addPipe();
    bool running = true;
    bool gameOver = false;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN) {
                if (!gameOver && event.key.keysym.sym == SDLK_SPACE) {
                    bird.velocity = -8; // Chim nhảy lên
                }
                else if (gameOver) {
                    running = false;
                }
            }
        }

        if (!gameOver) {
            updateBird();
            updatePipes();

            for (auto &pipe : pipes) {
                if (checkCollision(bird, pipe)) {
                    gameOver = true;
                }
                // Kiểm tra xem chim đã vượt qua ống chưa
                if (bird.x > pipe.x + PIPE_WIDTH && !gameOver) {
                    score++;
                }
            }
        }

        SDL_RenderCopy(renderer, background, NULL, NULL);
        renderPipes(renderer);
        drawBird(renderer);
        drawScore(renderer); // Hiển thị điểm số

        if (gameOver) {
            drawGameOver(renderer);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(20);
    }

    // Giải phóng bộ nhớ
    SDL_DestroyTexture(scoreTexture);
    TTF_CloseFont(font);
    TTF_Quit();

    SDL_DestroyTexture(gameOverTexture);
    SDL_DestroyTexture(birdTexture);
    SDL_DestroyTexture(pipeTexture);
    SDL_DestroyTexture(background);
    quitSDL(window, renderer);

    return 0;
}
