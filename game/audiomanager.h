#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <SDL_mixer.h>
#include <string>
#include <map> //để lưu trữ nhạc và âm thanh theo ID
using namespace std;

class AudioManager
{
public:
    //Trả về tham chiếu tới một thể hiện duy nhất (singleton) của AudioManager.
    //Điều này đảm bảo toàn bộ chương trình chỉ dùng một bộ quản lý âm thanh, tránh xung đột hoặc lãng phí tài nguyên.
    static AudioManager& getInstance();

    bool init();  // Khởi tạo SDL_mixer
    void close(); // Giải phóng tài nguyên

    bool loadMusic(const string& id, const string& filename);
    //Tải file nhạc nền (*.mp3, *.ogg) và lưu vào musicMap
    bool loadSound(const string& id, const string& filename);
    //Tải hiệu ứng âm thanh ngắn (*.wav, *.ogg) và lưu vào soundMap
    void playMusic(const string& id, int loops = -1);
    //loops: số lần lặp, -1:lặp vô hạn nếu không truyền gì
    void stopMusic();
    void pauseMusic();
    void resumeMusic();

    void playSound(const string& id, int loops = 0);

private:
    AudioManager() {}
    map<string, Mix_Music*> musicMap;
    //Đây là bản đồ (dictionary) ánh xạ từ tên nhạc (string) đến con trỏ Mix_Music*
    //Sau này chỉ cần gọi musicMap["theme"] là lấy được bản nhạc để phát.
    map<string, Mix_Chunk*> soundMap;
    //Tương tự như musicMap, nhưng dùng để lưu âm thanh hiệu ứng (sfx).

};

#endif


