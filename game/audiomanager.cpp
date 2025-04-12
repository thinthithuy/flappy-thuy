#include "AudioManager.h"
#include <SDL.h>
#include <iostream>

// Lấy thể hiện duy nhất của AudioManager (Singleton)
AudioManager& AudioManager::getInstance()
{
    static AudioManager instance;
    return instance;
}

// Khởi tạo SDL_mixer
bool AudioManager::init()
{   //Mix_OpenAudio() để khởi tạo SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }
    return true;
}

// Giải phóng tất cả tài nguyên âm thanh
void AudioManager::close()
{
    for (auto& music : musicMap)
    {
        Mix_FreeMusic(music.second);
    }
    for (auto& sound : soundMap)
    {
        Mix_FreeChunk(sound.second);
    }
    musicMap.clear();
    soundMap.clear();
    Mix_CloseAudio();
}

// Load nhạc nền
bool AudioManager::loadMusic(const string& id, const string& filename)
{
    Mix_Music* music = Mix_LoadMUS(filename.c_str());
    if (!music)
    {
        cerr << "Failed to load music " << filename << " Error: " << Mix_GetError() << endl;
        return false;
    }
    musicMap[id] = music;
    return true;
}

// Load hiệu ứng âm thanh
bool AudioManager::loadSound(const string& id, const string& filename)
{
    Mix_Chunk* sound = Mix_LoadWAV(filename.c_str());
    if (!sound)
    {
        cerr << "Failed to load sound " << filename << " Error: " << Mix_GetError() << endl;
        return false;
    }
    soundMap[id] = sound;
    return true;
}

// Phát nhạc nền
void AudioManager::playMusic(const string& id, int loops)
{
    if (musicMap.find(id) != musicMap.end())
    {
        Mix_PlayMusic(musicMap[id], loops);
    }
}

// Phát hiệu ứng âm thanh
void AudioManager::playSound(const string& id, int loops)
{
    if (soundMap.find(id) != soundMap.end())
    {
        Mix_PlayChannel(-1, soundMap[id], loops);
    }
}

