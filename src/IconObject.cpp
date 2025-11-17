#include "IconObject.h"
#include "TextureManager.h"
#include <iostream>

IconObject::IconObject(String name, int textureIndex) : AGameObject(name)
{
	this->textureIndex = textureIndex;
}

void IconObject::initialize()
{
	sf::Texture* texture = TextureManager::getInstance()->getStreamTextureFromList(this->textureIndex);

	if (texture != nullptr && this->sprite != nullptr)
	{
		this->sprite->setTexture(*texture);
		std::cout << "IconObject loaded texture index: " << this->textureIndex << std::endl;
	}
	else
	{
		std::cout << "Failed to load texture at index: " << this->textureIndex << std::endl;
	}
}


void IconObject::processInput(sf::Event event)
{
}

void IconObject::update(sf::Time deltaTime)
{
}

void IconObject::setTransparency(int alpha)
{
	if (this->sprite != nullptr)
	{
		sf::Color color = this->sprite->getColor();
		color.a = static_cast<sf::Uint8>(alpha);
		this->sprite->setColor(color);
	}
}