#pragma once
#include "AGameObject.h"
#include "LoadAssetThread.h"

#include <mutex>
#include "ThreadPool.h"
#include "AnimatedCharacter.h"
#include "LoadingText.h"
#include "PokeballAnimation.h"


class IconObject;

class TextureDisplay : public AGameObject, public IExecutionEvent
{
public:
	TextureDisplay();
	~TextureDisplay();
	void initialize();
	void processInput(sf::Event event);
	void update(sf::Time deltaTime);
	void OnFinishedExecution() override;

private:
	typedef std::vector<IconObject*> IconList;
	IconList iconList;

	ThreadPool threadPool = ThreadPool(30);
	AnimatedCharacter* loadingCharacter = nullptr;
	LoadingText* loadingText = nullptr;

	const int TOTAL_TEXTURES = 620;
	const float STREAMING_LOAD_DELAY = 500.0f;
	float ticks = 0.0f;

	int columnGrid = 0;
	int rowGrid = 0;

	const int MAX_COLUMN = 28;
	const int MAX_ROW = 22;

	std::mutex guard;

	// NEW SEQUENCE FLAGS
	bool loadingComplete = false;           // Set when all textures loaded
	bool pokeballAnimStarted = false;       // Pokeball animation begins
	bool pokeballAnimComplete = false;      // Pokeball animation finishes
	bool bgTransitionStarted = false;       // Background fade begins
	bool bgTransitionComplete = false;      // Background fade finishes
	bool allIconsVisible = false;           // Icon fade-in complete

	PokeballAnimation* pokeballAnim = nullptr;

	float bgTransitionTimer = 0.0f;
	const float BG_TRANSITION_DURATION = 2.0f;  // Match BGObject fade duration

	float delayTimer = 0.0f;
	const float ICON_FADE_DELAY = 0.5f;
	const float ICON_FADE_DURATION = 1.5f;
	float iconFadeProgress = 0.0f;

	bool shouldStartScrolling = false;
	bool isScrolling = false;
	float scrollTimer = 0.0f;
	float currentScrollOffset = 0.0f;

	const float SCROLL_DELAY = 2.0f;        // Wait 2 seconds before scrolling
	const float SCROLL_DURATION = 3.0f;     // Scroll animation takes 3 seconds
	const float TOTAL_SCROLL_DISTANCE = 680.0f; // 4 rows × 68 pixels = 272 pixels

	void updateScrollAnimation(sf::Time deltaTime);

	void spawnObject();
	void updateLoadingProgress();
	void startPokeballAnimation();          // NEW
	void startBackgroundTransition();        // NEW
	void updateIconFadeIn(sf::Time deltaTime);
};