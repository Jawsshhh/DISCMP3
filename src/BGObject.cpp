#include "BGObject.h"
#include <iostream>
#include "TextureManager.h"
#include "BaseRunner.h"

BGObject::BGObject(string name) : AGameObject(name)
{
}

BGObject::~BGObject()
{
    if (bg2Sprite != nullptr)
    {
        delete bg2Sprite;
        bg2Sprite = nullptr;
    }

    if (whiteOverlay != nullptr)
    {
        delete whiteOverlay;
        whiteOverlay = nullptr;
    }
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

    // Create white overlay sprite for fade effect
    whiteOverlay = new sf::RectangleShape(sf::Vector2f(BaseRunner::WINDOW_WIDTH, BaseRunner::WINDOW_HEIGHT));
    whiteOverlay->setFillColor(sf::Color(255, 255, 255, 0)); // Start transparent
    whiteOverlay->setPosition(0, 0);
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

    // Draw bg2 underneath white overlay during transition
    if (isFading && bg2Sprite != nullptr && fadeProgress >= 0.5f)
    {
        targetWindow->draw(*bg2Sprite);
    }

    // Draw white overlay on top
    if (isFading && whiteOverlay != nullptr)
    {
        targetWindow->draw(*whiteOverlay);
    }
}

void BGObject::startTransitionToBg2()
{
    if (bg2Texture == nullptr || bg2Sprite == nullptr)
    {
        std::cout << "Cannot transition - bg2 not loaded!" << std::endl;
        return;
    }

    std::cout << "Starting white fade transition to bg2..." << std::endl;
    isFading = true;
    fadeProgress = 0.0f;

    // Make bg2 fully visible (it will be revealed as white fades)
    sf::Color color = bg2Sprite->getColor();
    color.a = 255;
    bg2Sprite->setColor(color);
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

        // Make white overlay invisible
        whiteOverlay->setFillColor(sf::Color(255, 255, 255, 0));

        std::cout << "Fade to bg2 complete!" << std::endl;
    }
    else
    {
        // White fade effect: fade to white in first half, fade from white in second half
        if (fadeProgress <= 0.5f)
        {
            // First half: fade TO white (alpha increases)
            float alpha = (fadeProgress / 0.5f) * 255.0f;
            whiteOverlay->setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
        }
        else
        {
            // Second half: fade FROM white (alpha decreases)
            float alpha = ((1.0f - fadeProgress) / 0.5f) * 255.0f;
            whiteOverlay->setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
        }
    }
}