#include "TextureDisplay.h"
#include <iostream>
#include "TextureManager.h"
#include "BaseRunner.h"
#include "GameObjectManager.h"
#include "IconObject.h"
#include "BGObject.h"

TextureDisplay::TextureDisplay() : AGameObject("TextureDisplay")
{
}

TextureDisplay::~TextureDisplay()
{
	// Clean up icons
	for (auto icon : iconList)
	{
		delete icon;
	}

	if (loadingCharacter != nullptr)
	{
		delete loadingCharacter;
	}
	if (loadingText != nullptr)
	{
		delete loadingText;
	}
	if (pokeballAnim != nullptr)
	{
		delete pokeballAnim;
	}
}

void TextureDisplay::initialize()
{
	TextureManager::getInstance()->initializeStreamTextureList(TOTAL_TEXTURES);

	threadPool.StartScheduling();

	loadingCharacter = new AnimatedCharacter("LoadingCharacter");
	GameObjectManager::getInstance()->addObject(loadingCharacter);

	std::cout << "Loading character created" << std::endl;

	loadingText = new LoadingText("LoadingText");
	GameObjectManager::getInstance()->addObject(loadingText);

	std::cout << "Loading text created" << std::endl;
}

void TextureDisplay::processInput(sf::Event event)
{
}

void TextureDisplay::update(sf::Time deltaTime)
{
	this->ticks += deltaTime.asMilliseconds();

	if (this->ticks > this->STREAMING_LOAD_DELAY) {
		this->ticks = 0.0f;

		int spawnedIconCount = this->iconList.size();
		int loadedTextureCount = TextureManager::getInstance()->getNumLoadedStreamTextures();
		int numReadyToSpawn = loadedTextureCount - spawnedIconCount;

		// Only spawn if we have LOADED textures ready
		if (numReadyToSpawn > 0) {
			const int SPAWN_BATCH_SIZE = 20;
			int numToSpawnThisBatch = std::min(numReadyToSpawn, SPAWN_BATCH_SIZE);

			std::cout << "[MainThread] === Spawning BATCH of " << numToSpawnThisBatch
				<< " icons (out of " << numReadyToSpawn << " ready) ===" << std::endl;

			for (int i = 0; i < numToSpawnThisBatch; i++) {
				// Only spawn if the texture index exists
				int textureIndex = this->iconList.size();
				if (textureIndex < loadedTextureCount)
				{
					this->spawnObject();
				}
			}
		}

		updateLoadingProgress();

		// Load textures in batches
		const int LOAD_BATCH_SIZE = 40;
		int totalTextures = TOTAL_TEXTURES;

		if (loadedTextureCount < totalTextures)
		{
			int texturesToLoad = std::min(LOAD_BATCH_SIZE, totalTextures - loadedTextureCount);

			std::cout << "[MainThread] === Scheduling BATCH of " << texturesToLoad
				<< " texture loads ===" << std::endl;

			for (int i = 0; i < texturesToLoad; i++)
			{
				int textureIndex = loadedTextureCount + i;
				LoadAssetThread* asset = new LoadAssetThread(textureIndex, this);
				threadPool.ScheduleTask(asset);
			}
		}
	}

	// Track background transition timer
	if (loadingComplete && !bgTransitionComplete)
	{
		bgTransitionTimer += deltaTime.asSeconds();
		if (bgTransitionTimer >= BG_TRANSITION_DURATION)
		{
			bgTransitionComplete = true;
			std::cout << "Background transition complete! Ready for Pokeball..." << std::endl;
		}
	}

	if (bgTransitionComplete && !allIconsVisible)
	{
		updateIconFadeIn(deltaTime);
	}
}

void TextureDisplay::OnFinishedExecution()
{
	std::cout << "[LoadThread] Texture loaded. Total: "
		<< TextureManager::getInstance()->getNumLoadedStreamTextures() << std::endl;
}

void TextureDisplay::spawnObject()
{
	guard.lock();

	String objectName = "Icon_" + std::to_string(this->iconList.size());
	IconObject* iconObj = new IconObject(objectName, this->iconList.size());
	this->iconList.push_back(iconObj);

	// Set position in grid with offset
	int IMG_WIDTH = 68;
	int IMG_HEIGHT = 68;
	int OFFSET_X = -65;    
	int OFFSET_Y = -100;   

	float x = OFFSET_X + (this->columnGrid * IMG_WIDTH);
	float y = OFFSET_Y + (this->rowGrid * IMG_HEIGHT);

	std::cout << "Set position: " << x << " " << y << std::endl;

	this->columnGrid++;
	if (this->columnGrid == this->MAX_COLUMN)
	{
		this->columnGrid = 0;
		this->rowGrid++;
	}

	GameObjectManager::getInstance()->addObject(iconObj);
	iconObj->setPosition(x, y);
	iconObj->setTransparency(0);  

	guard.unlock();
}

void TextureDisplay::updateLoadingProgress()
{
	if (loadingCharacter == nullptr) return;

	int loadedCount = TextureManager::getInstance()->getNumLoadedStreamTextures();
	float progress = static_cast<float>(loadedCount) / static_cast<float>(TOTAL_TEXTURES);
	loadingCharacter->updateProgress(progress);

	// Start background transition when loading is complete
	if (loadedCount >= TOTAL_TEXTURES && !loadingComplete)
	{
		std::cout << "Loading complete! Starting background transition..." << std::endl;

		loadingComplete = true;

		// Start background transition
		BGObject* bgObject = dynamic_cast<BGObject*>(
			GameObjectManager::getInstance()->findObjectByName("BGObject")
			);
		if (bgObject != nullptr)
		{
			bgObject->startTransitionToBg2();
		}

		if (loadingCharacter != nullptr)
		{
			GameObjectManager::getInstance()->deleteObjectByName("LoadingCharacter");
			loadingCharacter = nullptr;
			std::cout << "Removed loading character" << std::endl;
		}

		if (loadingText != nullptr)
		{
			GameObjectManager::getInstance()->deleteObjectByName("LoadingText");
			loadingText = nullptr;
			std::cout << "Removed loading text" << std::endl;
		}
	}
}

void TextureDisplay::updateIconFadeIn(sf::Time deltaTime)
{
	if (pokeballAnim == nullptr && !pokeballAnimComplete)
	{
		pokeballAnim = new PokeballAnimation("PokeballAnim");
		GameObjectManager::getInstance()->addObject(pokeballAnim);
		std::cout << "Pokeball animation started!" << std::endl;
		return;
	}

	if (pokeballAnim != nullptr && !pokeballAnimComplete)
	{
		if (pokeballAnim->isComplete())
		{
			pokeballAnimComplete = true;
			GameObjectManager::getInstance()->deleteObjectByName("PokeballAnim");
			pokeballAnim = nullptr;
			std::cout << "Pokeball animation removed, starting icon fade" << std::endl;
		}
		return;
	}

	if (delayTimer < ICON_FADE_DELAY)
	{
		delayTimer += deltaTime.asSeconds();
		return;
	}

	// Fade in icons
	iconFadeProgress += deltaTime.asSeconds() / ICON_FADE_DURATION;

	if (iconFadeProgress >= 1.0f)
	{
		iconFadeProgress = 1.0f;
		allIconsVisible = true;
		std::cout << "Icon fade-in complete!" << std::endl;
	}

	// Update all icon transparencies
	int alpha = static_cast<int>(iconFadeProgress * 255);
	for (auto icon : iconList)
	{
		icon->setTransparency(alpha);
	}
}