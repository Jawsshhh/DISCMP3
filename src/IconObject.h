#pragma once
#include "AGameObject.h"

class IconObject : public AGameObject
{
public:
	IconObject(String name, int textureIndex);
	void initialize() override;
	void processInput(sf::Event event) override;
	void update(sf::Time deltaTime) override;
	void setTransparency(int alpha);  // ADD THIS

	
private:
	int textureIndex;
};