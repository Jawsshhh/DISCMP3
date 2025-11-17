#pragma once
#include <SFML/Audio.hpp>
#include <string>

class MusicManager
{
public:
    typedef std::string String;

    static MusicManager* getInstance();

    void loadMusic(const String& filepath);
    void play();
    void pause();
    void stop();
    void setVolume(float volume); // 0-100
    void setLoop(bool loop);
    bool isPlaying() const;

private:
    MusicManager() {};
    MusicManager(MusicManager const&) {};
    MusicManager& operator=(MusicManager const&) {};
    static MusicManager* sharedInstance;

    sf::Music music;
};