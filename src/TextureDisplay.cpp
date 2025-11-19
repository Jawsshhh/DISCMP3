#include "TextureDisplay.h"
#include <iostream>
#include "TextureManager.h"
#include "BaseRunner.h"
#include "GameObjectManager.h"
#include "IconObject.h"
#include "BGObject.h"

constexpr float BG_TRANSITION_DURATION_OVERRIDE = 1.0f; 

TextureDisplay::TextureDisplay() : AGameObject("TextureDisplay")
{
}

TextureDisplay::~TextureDisplay()
{
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

		if (numReadyToSpawn > 0 && !bgTransitionStarted) {
			const int SPAWN_BATCH_SIZE = 30;
			int numToSpawnThisBatch = std::min(numReadyToSpawn, SPAWN_BATCH_SIZE);

			std::cout << "[MainThread] === Spawning BATCH of " << numToSpawnThisBatch
				<< " icons (out of " << numReadyToSpawn << " ready) ===" << std::endl;

			for (int i = 0; i < numToSpawnThisBatch; i++) {
				int textureIndex = this->iconList.size();
				if (textureIndex < loadedTextureCount)
				{
					this->spawnObject();
				}
			}
		}

		updateLoadingProgress();

		const int LOAD_BATCH_SIZE = 30;
		int totalTextures = TOTAL_TEXTURES;

		if (loadedTextureCount < totalTextures && !bgTransitionStarted)
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


	if (loadingComplete && !pokeballAnimStarted)
	{
		startPokeballAnimation();
	}

	if (pokeballAnimStarted && !pokeballAnimComplete && pokeballAnim != nullptr)
	{
		if (pokeballAnim->isComplete())
		{
			pokeballAnimComplete = true;
			std::cout << "[TextureDisplay] Pokeball animation complete!" << std::endl;
			GameObjectManager::getInstance()->deleteObjectByName("PokeballAnim");
			pokeballAnim = nullptr;

			if (!bgTransitionStarted) {
				startBackgroundTransition();
			}
		}
	}

	if (pokeballAnimComplete && !bgTransitionStarted)
	{
		startBackgroundTransition();
	}

	if (bgTransitionStarted && !bgTransitionComplete)
	{
		bgTransitionTimer += deltaTime.asSeconds();
		if (bgTransitionTimer >= BG_TRANSITION_DURATION_OVERRIDE)
		{
			bgTransitionComplete = true;
			std::cout << "Background transition complete! Starting icon fade..." << std::endl;
		}
	}

	if (bgTransitionComplete && !allIconsVisible)
	{
		updateIconFadeIn(deltaTime);
	}


	if (allIconsVisible && !shouldStartScrolling)
	{
		shouldStartScrolling = true;
		std::cout << "All icons visible, will start scrolling soon..." << std::endl;
	}

	if (shouldStartScrolling)
	{
		updateScrollAnimation(deltaTime);
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

	
	int spawnedCount = static_cast<int>(iconList.size());

	// Only mark loading as complete when:
	//  - all textures are loaded, AND
	//  - all icon objects have been spawned into iconList.
	if (loadedCount >= TOTAL_TEXTURES && spawnedCount >= TOTAL_TEXTURES && !loadingComplete)
	{
		std::cout << "Loading complete! All textures loaded and all icons spawned. Ready for pokeball animation..." << std::endl;
		loadingComplete = true;

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
	else if (loadedCount >= TOTAL_TEXTURES && spawnedCount < TOTAL_TEXTURES)
	{
		std::cout << "All textures are loaded (" << loadedCount << "), waiting for spawned icons (" << spawnedCount << "/" << TOTAL_TEXTURES << ") before proceeding." << std::endl;
	}
}

void TextureDisplay::startPokeballAnimation()
{
	if (pokeballAnim == nullptr)
	{
		pokeballAnim = new PokeballAnimation("PokeballAnim");
		GameObjectManager::getInstance()->addObject(pokeballAnim);
		pokeballAnimStarted = true;
		std::cout << "Pokeball animation started!" << std::endl;
	}
}

void TextureDisplay::startBackgroundTransition()
{
	std::cout << "Starting background transition..." << std::endl;

	BGObject* bgObject = dynamic_cast<BGObject*>(
		GameObjectManager::getInstance()->findObjectByName("BGObject")
		);

	if (bgObject != nullptr)
	{
		bgObject->startTransitionToBg2();
		bgTransitionStarted = true;
		bgTransitionTimer = 0.0f;
	}
}

void TextureDisplay::updateIconFadeIn(sf::Time deltaTime)
{
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

	//   icon transparencies
	int alpha = static_cast<int>(iconFadeProgress * 255);
	for (auto icon : iconList)
	{
		icon->setTransparency(alpha);
	}
}

void TextureDisplay::updateScrollAnimation(sf::Time deltaTime)
{
	if (!isScrolling)
	{
		scrollTimer += deltaTime.asSeconds();

		if (scrollTimer >= SCROLL_DELAY)
		{
			isScrolling = true;
			scrollTimer = 0.0f;
			std::cout << "Starting scroll animation..." << std::endl;
		}
		return;
	}

	// Animate scrolling
	scrollTimer += deltaTime.asSeconds();
	float progress = std::min(scrollTimer / SCROLL_DURATION, 1.0f);

	// Smooth easing (ease-in-out)
	float easedProgress = progress < 0.5f
		? 2.0f * progress * progress
		: 1.0f - std::pow(-2.0f * progress + 2.0f, 2.0f) / 2.0f;

	currentScrollOffset = easedProgress * TOTAL_SCROLL_DISTANCE;

	// Apply offset to all icons
	for (size_t i = 0; i < iconList.size(); i++)
	{
		int col = i % MAX_COLUMN;
		int row = i / MAX_COLUMN;

		float baseX = -65 + (col * 68);
		float baseY = -100 + (row * 68);

		iconList[i]->setPosition(baseX, baseY - currentScrollOffset);
	}

	if (progress >= 1.0f)
	{
		std::cout << "Scroll animation complete! All 620 icons shown." << std::endl;
	}
}