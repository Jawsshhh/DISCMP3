#include "FPSCounter.h"
#include <iostream>
#include "BaseRunner.h"

FPSCounter::FPSCounter() : AGameObject("FPSCounter")
{
}

FPSCounter::~FPSCounter()
{
	if (this->statsText != nullptr)
	{
		delete this->statsText;
		this->statsText = nullptr;
	}

	if (this->font != nullptr)
	{
		delete this->font;
		this->font = nullptr;
	}
	// DON'T call parent destructor explicitly - it's called automatically
}

void FPSCounter::initialize()
{
	this->font = new sf::Font();
	if (!this->font->loadFromFile("Media/Sansation.ttf"))
	{
		std::cout << "Warning: Failed to load font for FPS counter!" << std::endl;
		return;
	}

	this->statsText = new sf::Text();
	this->statsText->setFont(*this->font);
	this->statsText->setPosition(BaseRunner::WINDOW_WIDTH - 150, BaseRunner::WINDOW_HEIGHT - 70);
	this->statsText->setOutlineColor(sf::Color::White);  // Better syntax
	this->statsText->setOutlineThickness(2.5f);
	this->statsText->setCharacterSize(35);
	this->statsText->setFillColor(sf::Color::White);  // Add this to set text color
}

void FPSCounter::processInput(sf::Event event)
{
}

void FPSCounter::update(sf::Time deltaTime)
{
	this->updateFPS(deltaTime);
}

void FPSCounter::draw(sf::RenderWindow* targetWindow)
{
	AGameObject::draw(targetWindow);

	if (this->statsText != nullptr)
		targetWindow->draw(*this->statsText);
}

void FPSCounter::updateFPS(sf::Time elapsedTime)
{
	this->updateTime += elapsedTime;
	this->framesPassed++;

	if (this->updateTime >= sf::seconds(1.0f))
	{
		float fps = this->framesPassed / this->updateTime.asSeconds();

		this->statsText->setString("FPS: " + std::to_string((int)fps));

		this->updateTime = sf::Time::Zero;
		this->framesPassed = 0;
	}
}