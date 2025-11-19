#pragma once
#include "AGameObject.h"
#include <vector>

class AnimatedCharacter : public AGameObject
{
public:
    AnimatedCharacter(String name);
    void initialize() override;
    void processInput(sf::Event event) override;
    void update(sf::Time deltaTime) override;

    void updateProgress(float progress); // 0.0 to 1.0

private:
    std::vector<sf::Texture*> animationFrames;
    int currentFrame = 0;
    float animationSpeed = 0.075f; // Time between frames
    float timeSinceLastFrame = 0.0f;

    float startX = 100.0f;
    float endX = 1820.0f; // Near the right edge
    float yPosition = 600.0f; // Near bottom of screen

    void loadAnimationFrames();
    void updateAnimation(sf::Time deltaTime);
};