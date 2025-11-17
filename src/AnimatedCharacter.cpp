#include "AnimatedCharacter.h"
#include "TextureManager.h"
#include "BaseRunner.h"
#include <iostream>

AnimatedCharacter::AnimatedCharacter(String name) : AGameObject(name)
{
}

void AnimatedCharacter::initialize()
{
    std::cout << "Initializing animated character" << std::endl;

    loadAnimationFrames();

    // Set initial position
    this->setPosition(startX, yPosition);

    if (!animationFrames.empty() && this->sprite != nullptr)
    {
        this->sprite->setTexture(*animationFrames[0]);
        this->sprite->setScale(2.0f, 2.0f); // Make it bigger if needed
    }
}

void AnimatedCharacter::loadAnimationFrames()
{
    

    sf::Texture* frame1 = TextureManager::getInstance()->getFromTextureMap("tile000", 0);
    if (frame1) animationFrames.push_back(frame1);

    sf::Texture* frame2 = TextureManager::getInstance()->getFromTextureMap("tile001", 0);
    if (frame2) animationFrames.push_back(frame2);

    sf::Texture* frame3 = TextureManager::getInstance()->getFromTextureMap("tile002", 0);
    if (frame3) animationFrames.push_back(frame3);

    sf::Texture* frame4 = TextureManager::getInstance()->getFromTextureMap("tile003", 0);
    if (frame4) animationFrames.push_back(frame4);


    if (animationFrames.empty())
    {
        std::cout << "Warning: No animation frames loaded!" << std::endl;
    }
    else
    {
        std::cout << "Loaded " << animationFrames.size() << " animation frames" << std::endl;
    }
}

void AnimatedCharacter::processInput(sf::Event event)
{
}

void AnimatedCharacter::update(sf::Time deltaTime)
{
    updateAnimation(deltaTime);
}

void AnimatedCharacter::updateAnimation(sf::Time deltaTime)
{
    if (animationFrames.empty()) return;

    timeSinceLastFrame += deltaTime.asSeconds();

    if (timeSinceLastFrame >= animationSpeed)
    {
        // Move to next frame
        currentFrame = (currentFrame + 1) % animationFrames.size();

        if (this->sprite != nullptr)
        {
            this->sprite->setTexture(*animationFrames[currentFrame]);
        }

        timeSinceLastFrame = 0.0f;
    }
}

void AnimatedCharacter::updateProgress(float progress)
{
    // progress should be 0.0 to 1.0
    // Move character from start to end position
    float currentX = startX + (endX - startX) * progress;
    this->setPosition(currentX, yPosition);
}