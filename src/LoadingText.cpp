#include "LoadingText.h"
#include "TextureManager.h"
#include "BaseRunner.h"
#include <iostream>

LoadingText::LoadingText(String name) : AGameObject(name)
{
}

void LoadingText::initialize()
{
    std::cout << "Initializing loading text animation" << std::endl;

    loadAnimationFrames();

    // Set initial position
    this->setPosition(xPosition, yPosition);

    // Set up sprite with first frame if available
    if (!animationFrames.empty() && this->sprite != nullptr)
    {
        this->sprite->setTexture(*animationFrames[0]);
        // Optionally scale it up/down
        // this->sprite->setScale(2.0f, 2.0f);
    }
}

void LoadingText::loadAnimationFrames()
{
    // Load all 6 frames of your loading animation
    // Make sure these are added to assets.txt!

    sf::Texture* frame1 = TextureManager::getInstance()->getFromTextureMap("loading1", 0);
    if (frame1) animationFrames.push_back(frame1);

    sf::Texture* frame2 = TextureManager::getInstance()->getFromTextureMap("loading2", 0);
    if (frame2) animationFrames.push_back(frame2);

    sf::Texture* frame3 = TextureManager::getInstance()->getFromTextureMap("loading3", 0);
    if (frame3) animationFrames.push_back(frame3);

    sf::Texture* frame4 = TextureManager::getInstance()->getFromTextureMap("loading4", 0);
    if (frame4) animationFrames.push_back(frame4);

    sf::Texture* frame5 = TextureManager::getInstance()->getFromTextureMap("loading5", 0);
    if (frame5) animationFrames.push_back(frame5);

    sf::Texture* frame6 = TextureManager::getInstance()->getFromTextureMap("loading6", 0);
    if (frame6) animationFrames.push_back(frame6);

    if (animationFrames.empty())
    {
        std::cout << "Warning: No loading text frames loaded!" << std::endl;
    }
    else
    {
        std::cout << "Loaded " << animationFrames.size() << " loading text frames" << std::endl;
    }
}

void LoadingText::processInput(sf::Event event)
{
}

void LoadingText::update(sf::Time deltaTime)
{
    updateAnimation(deltaTime);
}

void LoadingText::updateAnimation(sf::Time deltaTime)
{
    if (animationFrames.empty()) return;

    timeSinceLastFrame += deltaTime.asSeconds();

    if (timeSinceLastFrame >= animationSpeed)
    {
        // Move to next frame (loop back to start)
        currentFrame = (currentFrame + 1) % animationFrames.size();

        if (this->sprite != nullptr)
        {
            this->sprite->setTexture(*animationFrames[currentFrame]);
        }

        timeSinceLastFrame = 0.0f;
    }
}