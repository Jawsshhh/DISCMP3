#pragma once
#include "AGameObject.h"
#include <vector>

class PokeballAnimation : public AGameObject
{
public:
    PokeballAnimation(String name);
    void initialize() override;
    void processInput(sf::Event event) override;
    void update(sf::Time deltaTime) override;

    bool isComplete() const { return animationComplete; }

private:
    std::vector<sf::Texture*> animationFrames;
    int currentFrame = 0;
    float animationSpeed = 0.095f;
    float timeSinceLastFrame = 0.0f;
    bool animationComplete = false;

    float completionTimer = 0.0f;
    const float HOLD_LAST_FRAME_DURATION = 0.5f;  // Hold last frame for 0.5 seconds

    float xPosition = 960.0f;
    float yPosition = 540.0f;

    void loadAnimationFrames();
    void updateAnimation(sf::Time deltaTime);
};