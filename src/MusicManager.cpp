#include "MusicManager.h"
#include <iostream>

MusicManager* MusicManager::sharedInstance = nullptr;

MusicManager* MusicManager::getInstance()
{
    if (sharedInstance == nullptr)
    {
        sharedInstance = new MusicManager();
    }
    return sharedInstance;
}

void MusicManager::loadMusic(const String& filepath)
{
    if (!music.openFromFile(filepath))
    {
        std::cout << "[MusicManager] Failed to load music: " << filepath << std::endl;
    }
    else
    {
        std::cout << "[MusicManager] Loaded music: " << filepath << std::endl;
    }
}

void MusicManager::play()
{
    music.play();
}

void MusicManager::pause()
{
    music.pause();
}

void MusicManager::stop()
{
    music.stop();
}

void MusicManager::setVolume(float volume)
{
    music.setVolume(volume);
}

void MusicManager::setLoop(bool loop)
{
    music.setLoop(loop);
}

bool MusicManager::isPlaying() const
{
    return music.getStatus() == sf::Music::Playing;
}