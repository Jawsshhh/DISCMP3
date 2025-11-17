#pragma once
#include "AGameObject.h"

class BGObject : public AGameObject
{
public:
    BGObject(String name);
    void initialize();
    void processInput(sf::Event event);
    void update(sf::Time deltaTime);
    void draw(sf::RenderWindow* targetWindow) override;
    void startTransitionToBg2();  // ADD THIS - call when loading completes

private:
    const float SPEED_MULTIPLIER = 500.0f;
    const float FADE_DURATION = 2.0f; // Fade takes 2 seconds

    sf::Texture* bg1Texture = nullptr;
    sf::Texture* bg2Texture = nullptr;
    sf::Sprite* bg2Sprite = nullptr;

    bool isFading = false;
    float fadeProgress = 0.0f;

    void updateFade(float deltaTime);
};