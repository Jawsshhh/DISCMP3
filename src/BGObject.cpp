#include "BGObject.h"
#include <iostream>
#include "TextureManager.h"
#include "BaseRunner.h"

BGObject::BGObject(string name) : AGameObject(name)
{
}

void BGObject::initialize()
{
    std::cout << "Declared as " << this->getName() << "\n";

    // Load bg1
    bg1Texture = TextureManager::getInstance()->getFromTextureMap("bg1", 0);

    if (bg1Texture != nullptr)
    {
        bg1Texture->setRepeated(true);
        this->sprite->setTexture(*bg1Texture);
        this->sprite->setTextureRect(sf::IntRect(0, 0, BaseRunner::WINDOW_WIDTH, BaseRunner::WINDOW_HEIGHT * 8));
        this->setPosition(0, -BaseRunner::WINDOW_HEIGHT * 7);
    }
    else
    {
        std::cout << "Warning: Failed to load bg1!" << std::endl;
    }

    // Load bg2 and prepare sprite
    bg2Texture = TextureManager::getInstance()->getFromTextureMap("bg2", 0);

    if (bg2Texture != nullptr)
    {
        bg2Texture->setRepeated(true);
        bg2Sprite = new sf::Sprite();
        bg2Sprite->setTexture(*bg2Texture);
        bg2Sprite->setTextureRect(sf::IntRect(0, 0, BaseRunner::WINDOW_WIDTH, BaseRunner::WINDOW_HEIGHT * 8));
        bg2Sprite->setPosition(0, -BaseRunner::WINDOW_HEIGHT * 7);

        // Start invisible
        sf::Color color = bg2Sprite->getColor();
        color.a = 0;
        bg2Sprite->setColor(color);
    }
    else
    {
        std::cout << "Warning: Failed to load bg2!" << std::endl;
    }
}

void BGObject::processInput(sf::Event event)
{
}

void BGObject::update(sf::Time deltaTime)
{
    if (isFading)
    {
        updateFade(deltaTime.asSeconds());
    }
}

void BGObject::draw(sf::RenderWindow* targetWindow)
{
    // Draw bg1 first
    if (this->sprite != nullptr)
    {
        targetWindow->draw(*this->sprite);
    }

    // Draw bg2 on top if fading
    if (isFading && bg2Sprite != nullptr)
    {
        targetWindow->draw(*bg2Sprite);
    }
}

void BGObject::startTransitionToBg2()
{
    if (bg2Texture == nullptr || bg2Sprite == nullptr)
    {
        std::cout << "Cannot transition - bg2 not loaded!" << std::endl;
        return;
    }

    std::cout << "Starting fade transition to bg2..." << std::endl;
    isFading = true;
    fadeProgress = 0.0f;
}

void BGObject::updateFade(float deltaTime)
{
    fadeProgress += deltaTime / FADE_DURATION;

    if (fadeProgress >= 1.0f)
    {
        // Fade complete
        fadeProgress = 1.0f;
        isFading = false;

        // Swap to bg2 as main background
        this->sprite->setTexture(*bg2Texture);

        std::cout << "Fade to bg2 complete!" << std::endl;
    }
    else
    {
        // Update alpha for smooth fade
        sf::Color color = bg2Sprite->getColor();
        color.a = static_cast<sf::Uint8>(fadeProgress * 255);
        bg2Sprite->setColor(color);
    }
}