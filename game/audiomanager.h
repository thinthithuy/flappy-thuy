#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <SDL_mixer.h>
#include <string>
#include <map>

class AudioManager {
public:
    static AudioManager& getInstance(); // Singleton

    bool init();  // Khởi tạo SDL_mixer
    void close(); // Giải phóng tài nguyên

    bool loadMusic(const std::string& id, const std::string& filename);
    bool loadSound(const std::string& id, const std::string& filename);

    void playMusic(const std::string& id, int loops = -1);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();

    void playSound(const std::string& id, int loops = 0);

private:
    AudioManager() {} // Constructor private (Singleton)
    std::map<std::string, Mix_Music*> musicMap;
    std::map<std::string, Mix_Chunk*> soundMap;
};

#endif // AUDIOMANAGER_H
