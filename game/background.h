//nền trò chơi, ống nước, xử lý va chạm và hiển thị hình ảnh
#ifndef BACKGROUND.H
#define BACKGROUND.H

#include <SDL.h>       //cung cấp các chức năng đồ họa(vẽ ảnh, tạo cửa sổ, xử lý hình ảnh)
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib> //thu vien cho ham rand
#include "Bird.h"
#include "audiomanager.h"

using namespace std;

class Background //Khai báo lớp Background: có nhiệm vụ vẽ nền, ống nước, kiểm tra va chạm và quản lý điểm số
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
        int x, height; //x là tọa dộ ngang của ống nước, height là chiều cao của ống
        bool scored = false; //để xác định xem ống nước đã được tính điểm chưa
    };

public:

    void reset() //reset lại danh sách ống nước
    {
       pipes.clear(); //xóa toàn bộ các ống nước, chuẩn bị bắt đầu lại game
    }

    bool checkCollision(Bird& bird, const Pipe& pipe);

    void Init_Background(SDL_Renderer* renderer); //load các hình ảnh nền và chuẩn bị cho quá trình vẽ

    vector<Pipe> pipes; //danh sách chứa các ống nước xuất hiện trên màn hình

    void renderTexture(SDL_Texture* texture, int x,int y,SDL_Renderer* render); //vẽ 1 ảnh lên màn hình, hiển thị ảnh texture tại tọa độ (x,y)

    SDL_Texture* loadTexture(const char *filename, SDL_Renderer* render); //load 1 ảnh từ file vào dạng SDL_Texture*

    void drawPipe(int x, int height, SDL_Renderer* renderer); //vẽ 1 ống nước tại vị trí (x,height)

    void addPipe(); //thêm 1 ống nước mới

    void updatePipes(); //cập nhật vị trí ống nước, di chuyển tất cả ống nước sang trái theo PIPE_SPEED

    void renderPipes(SDL_Renderer* renderer); //hiển thị tất cả các ống nước, vẽ tất cả các ống nước lên màn hình

    void drawGameOver(SDL_Renderer* renderer);//hiển thị màn hình gameover khi trò chơi kết thúc

    int updateScore(bool gameOver,Bird& bird) //cập nhật điểm số
    {
        int score = 0;
        for (auto &pipe : pipes) //duyệt qua toàn bộ các ống nước trong danh sách
        {
            if (!gameOver && pipe.x + PIPE_WIDTH < bird.Get_x() && pipe.scored == false)
            {               //pipe.x la vi trí bên trái của ống, +thêm width thành vị trí mép phải của ống
                score++;    //bird.Get_x() là tọa độ x của con chim
                pipe.scored = true;
            }
        }
        return score;
    }                      //con trỏ đến bộ vẽ (render) của SDL
    void render_background(SDL_Renderer* render,Bird& bird); //hàm vẽ mọi thứ lên màn hình

    void check_coll(Bird& bird,AudioManager& audio,bool& gameOver); //kiểm tra va chạm

    void clear_texture() // giải phóng bộ nhớ các hình ảnh (texture) mà game đã nạp bằng SDL
    {
        SDL_DestroyTexture(gameOverTexture);
        SDL_DestroyTexture(birdTexture);
        SDL_DestroyTexture(pipeTexture);
        SDL_DestroyTexture(background);
    }
};

#endif

