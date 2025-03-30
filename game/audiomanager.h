#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <SDL_mixer.h>
#include <string>
#include <map>
using namespace std;

class AudioManager
{
public:
    static AudioManager& getInstance(); // Singleton

    bool init();  // Khởi tạo SDL_mixer
    void close(); // Giải phóng tài nguyên

    bool loadMusic(const string& id, const string& filename);
    bool loadSound(const string& id, const string& filename);

    void playMusic(const string& id, int loops = -1);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();

    void playSound(const string& id, int loops = 0);

private:
    AudioManager() {} // Constructor private (Singleton)
    map<string, Mix_Music*> musicMap;
    map<string, Mix_Chunk*> soundMap;
};

#endif


