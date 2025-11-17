#include "PokeballAnimation.h"
#include "TextureManager.h"
#include "BaseRunner.h"
#include <iostream>

PokeballAnimation::PokeballAnimation(String name) : AGameObject(name)
{
}

void PokeballAnimation::initialize()
{
    std::cout << "=== POKEBALL ANIMATION INITIALIZING ===" << std::endl;

    loadAnimationFrames();

    this->setPosition(xPosition, yPosition);

    if (!animationFrames.empty() && this->sprite != nullptr)
    {
        this->sprite->setTexture(*animationFrames[0]);

        sf::FloatRect bounds = this->sprite->getLocalBounds();
        this->sprite->setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);

        this->sprite->setScale(5.0f, 5.0f);

        std::cout << "Pokeball sprite set! Size: " << bounds.width << "x" << bounds.height << std::endl;
        std::cout << "Position: " << xPosition << ", " << yPosition << std::endl;
    }
    else
    {
        std::cout << "ERROR: Pokeball sprite is NULL or no frames!" << std::endl;
    }
}

void PokeballAnimation::loadAnimationFrames()
{
    std::cout << "Loading Pokeball frames..." << std::endl;

    sf::Texture* frame1 = TextureManager::getInstance()->getFromTextureMap("pokeball1", 0);
    if (frame1) {
        animationFrames.push_back(frame1);
        std::cout << "Loaded pokeball1" << std::endl;
    }
    else {
        std::cout << "ERROR: Failed to load pokeball1" << std::endl;
    }

    sf::Texture* frame2 = TextureManager::getInstance()->getFromTextureMap("pokeball2", 0);
    if (frame2) {
        animationFrames.push_back(frame2);
        std::cout << "Loaded pokeball2" << std::endl;
    }
    else {
        std::cout << "ERROR: Failed to load pokeball2" << std::endl;
    }

    sf::Texture* frame3 = TextureManager::getInstance()->getFromTextureMap("pokeball3", 0);
    if (frame3) {
        animationFrames.push_back(frame3);
        std::cout << "Loaded pokeball3" << std::endl;
    }
    else {
        std::cout << "ERROR: Failed to load pokeball3" << std::endl;
    }

    if (animationFrames.empty())
    {
        std::cout << "WARNING: No Pokeball frames loaded!" << std::endl;
    }
    else
    {
        std::cout << "SUCCESS: Loaded " << animationFrames.size() << " Pokeball frames" << std::endl;
    }
}

void PokeballAnimation::processInput(sf::Event event)
{
}

void PokeballAnimation::update(sf::Time deltaTime)
{
    if (!animationComplete)
    {
        updateAnimation(deltaTime);
    }
}

void PokeballAnimation::updateAnimation(sf::Time deltaTime)
{
    if (animationFrames.empty()) return;

    timeSinceLastFrame += deltaTime.asSeconds();

    if (timeSinceLastFrame >= animationSpeed)
    {
        currentFrame++;

        std::cout << "Pokeball frame: " << currentFrame << " / " << animationFrames.size() << std::endl;

        if (currentFrame >= animationFrames.size())
        {
            // Animation finished - stay on last frame for a bit longer
            currentFrame = animationFrames.size() - 1;
            animationComplete = true;
            std::cout << "=== POKEBALL ANIMATION COMPLETE ===" << std::endl;
        }

        if (this->sprite != nullptr)
        {
            this->sprite->setTexture(*animationFrames[currentFrame]);
        }

        timeSinceLastFrame = 0.0f;
    }
}