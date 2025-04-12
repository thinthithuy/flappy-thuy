#include "background.h"
#include "Bird.h"
using namespace std;


void Background::drawGameOver(SDL_Renderer* renderer)
{
    SDL_Rect dest = { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, 200, 100 }; // Điều chỉnh kích thước
    SDL_RenderCopy(renderer, gameOverTexture, NULL, &dest);
}
//load ảnh lên để cbi vẽ
void Background::Init_Background(SDL_Renderer* renderer)
{
    pipeTexture = loadTexture("pipe.png", renderer);
    birdTexture = loadTexture("bird.png", renderer);
    background = loadTexture("backgr.jpg", renderer);
    gameOverTexture = loadTexture("gameover.png", renderer);
}
//vẽ 1 ảnh lên màn hình tại vị trí x,y
void Background::renderTexture(SDL_Texture *texture, int x, int y, SDL_Renderer* renderer)
{
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);//lấy kích thước của texture tự động
    SDL_RenderCopy(renderer, texture, NULL, &dest);//vẽ texture lên vị trí dest
}
//Load ảnh từ file và trả về SDL_Texture*
SDL_Texture* Background::loadTexture(const char *filename, SDL_Renderer* renderer)
{
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename); //ghi log khi bắt đầu load ảnh
                            //load texture từ file ảnh
    SDL_Texture *texture = IMG_LoadTexture(renderer, filename);
    if (texture == NULL)
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Load texture %s", IMG_GetError());

    return texture;
}

void Background::drawPipe(int x, int height, SDL_Renderer* renderer)
{
    // Cột trên (lộn ngược)
    SDL_Rect topPipe = {x, 0, PIPE_WIDTH, height};
    SDL_Rect srcRect = {0, 0, 80, 400}; // Điều chỉnh theo ảnh thực tế, srcRect chọn phần ảnh cần hiển thị từ texture
    SDL_RenderCopyEx(renderer, pipeTexture, &srcRect, &topPipe, 0, NULL, SDL_FLIP_VERTICAL); //SDL_FLIP_VERTICAL: lật ảnh theo chiều dọc – tạo hiệu ứng ống nước "ngược".


    //SDL_RenderCopyEx để vẽ ảnh ống từ srcRect vào vị trí topPipe.

    // Cột dưới
    SDL_Rect bottomPipe = {x, height + PIPE_GAP, PIPE_WIDTH, SCREEN_HEIGHT - height - PIPE_GAP};
    SDL_RenderCopy(renderer, pipeTexture, &srcRect, &bottomPipe);
}

void Background::addPipe()
{                                                               //+50: đảm bảo ống trên không quá thấp (tối thiểu là 50 pixel từ trên xuống)
    int height = rand() % (SCREEN_HEIGHT - PIPE_GAP - 100) + 50;//SCREEN_HEIGHT - PIPE_GAP - 100: đảm bảo rằng ống dưới vẫn còn đủ chỗ, không đè lên ống trên.
    pipes.push_back({SCREEN_WIDTH, height});//Tạo một Pipe mới (dùng struct Pipe { int x, height; bool scored; })
}

void Background::updatePipes()
{
    for (auto &pipe : pipes)
    {
        pipe.x -= PIPE_SPEED; //làm cột chạy sang trái
    }

    if (!pipes.empty() && pipes[0].x < -PIPE_WIDTH)//xóa ống cũ nếu ra khỏi màn hình
    {
        pipes.erase(pipes.begin());
    }

    if (pipes.empty() || pipes.back().x < SCREEN_WIDTH - 250)//thêm ống mới nếu cần
    {                   //ống gần nhất ở bên phải
        addPipe();
    }
}

void Background::renderPipes(SDL_Renderer* renderer)//Vẽ từng ống nước (gồm cả ống trên và ống dưới) bằng hàm drawPipe().
{
    for (auto &pipe : pipes)
    {
        drawPipe(pipe.x, pipe.height, renderer);
    }
}
bool Background::checkCollision(Bird& bird, const Pipe& pipe)
{
    //Dùng SDL_Rect để biểu diễn vùng không gian của chim (gọi là bounding box).
    //Get_x() và Get_y() trả về tọa độ hiện tại của chim
    //50 x 50 là chiều rộng và chiều cao của hình chữ nhật bao quanh chim (có thể tương ứng với kích thước ảnh chim của bạn).
    SDL_Rect birdRect = {(int)bird.Get_x(), (int)bird.Get_y(), 50, 50}; // Hình chữ nhật của chim
    SDL_Rect topPipeRect = {pipe.x, 0, PIPE_WIDTH, pipe.height}; // Ống trên
    SDL_Rect bottomPipeRect = {pipe.x, pipe.height + PIPE_GAP, PIPE_WIDTH, SCREEN_HEIGHT - pipe.height - PIPE_GAP}; // Ống dưới

    // Kiểm tra va chạm với ống trên
    if (SDL_HasIntersection(&birdRect, &topPipeRect))//hàm SDL_HasIntersection để kiểm tra 2 hình chữ nhật có giao nhau không.
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
//ve anh nen, ong nc, con chim
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
