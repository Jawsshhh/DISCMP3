#include <fstream>
#include <iostream>
#include <filesystem>
#include "TextureManager.h"
#include "StringUtils.h"
#include "IETThread.h"

//a singleton class
TextureManager* TextureManager::sharedInstance = NULL;

TextureManager* TextureManager::getInstance() {
	if (sharedInstance == NULL) {
		//initialize
		sharedInstance = new TextureManager();
	}

	return sharedInstance;
}

TextureManager::TextureManager()
{
	this->countStreamingAssets();
}

void TextureManager::loadFromAssetList()
{
	std::cout << "[TextureManager] Reading from asset list" << std::endl;

	// Diagnostic: print current working directory and absolute target path
	std::cout << "[TextureManager] Current working directory: " << std::filesystem::current_path() << std::endl;
	auto assetsPath = std::filesystem::path("Media") / "assets.txt";
	std::cout << "[TextureManager] Attempting to open: " << std::filesystem::absolute(assetsPath) << std::endl;

	std::ifstream stream(assetsPath);
	if (!stream.is_open()) {
		std::cerr << "[TextureManager] ERROR: Failed to open assets file: " << std::filesystem::absolute(assetsPath) << std::endl;
		return;
	}

	String path;
	while (std::getline(stream, path))
	{
		// Trim whitespace? at least log the exact line read
		std::cout << "[TextureManager] assets.txt line: '" << path << "'" << std::endl;

		std::vector<String> tokens = StringUtils::split(path, '/');
		if (tokens.empty()) {
			continue;
		}
		String assetName = StringUtils::split(tokens[tokens.size() - 1], '.')[0];
		this->instantiateAsTexture(path, assetName, false);
		std::cout << "[TextureManager] Loaded texture: " << assetName << std::endl;
	}
}

void TextureManager::loadSingleStreamAsset(int index)
{
	int fileNum = 0;

	for (const auto& entry : std::filesystem::directory_iterator(STREAMING_PATH)) {
		if (index == fileNum)
		{
			auto filePath = entry.path();
			std::cout << filePath.filename() << std::endl;

			// Load as image first
			sf::Image image;
			if (!image.loadFromFile(filePath.string())) {
				std::cout << "Failed to load image" << std::endl;
				break;
			}

			// Create a new 256x256 image
			sf::Image resizedImage;
			resizedImage.create(256, 256);

			// Copy and scale pixels
			for (unsigned int y = 0; y < 256; y++) {
				for (unsigned int x = 0; x < 256; x++) {
					// Sample from original image (simple nearest-neighbor)
					resizedImage.setPixel(x, y, image.getPixel(x * 2, y * 2));
				}
			}

			// Create texture from resized image
			sf::Texture* texture = new sf::Texture();
			texture->loadFromImage(resizedImage);

			this->setStreamTextureAtIndex(index, texture);
			this->textureMap[filePath.filename().string()].push_back(texture);

			std::cout << "[TextureManager] Loaded and resized streaming texture at index " << index << std::endl;
			break;
		}

		fileNum++;
	}
}

sf::Texture* TextureManager::getFromTextureMap(const String assetName, int frameIndex)
{
	if (!this->textureMap[assetName].empty()) {
		return this->textureMap[assetName][frameIndex];
	}
	else {
		std::cout << "[TextureManager] No texture found for " << assetName << std::endl;
		return NULL;
	}
}

int TextureManager::getNumFrames(const String assetName)
{
	if (!this->textureMap[assetName].empty()) {
		return this->textureMap[assetName].size();
	}
	else {
		std::cout << "[TextureManager] No texture found for " << assetName << std::endl;
		return 0;
	}
}

sf::Texture* TextureManager::getStreamTextureFromList(const int index)
{
	return this->streamTextureList[index];
}

int TextureManager::getNumLoadedStreamTextures() const
{
	int count = 0;
	for (const auto& texture : this->streamTextureList)
	{
		if (texture != nullptr) count++;
	}
	return count;
}

void TextureManager::countStreamingAssets()
{
	this->streamingAssetCount = 0;
	for (const auto& entry : std::filesystem::directory_iterator(STREAMING_PATH)) {
		this->streamingAssetCount++;
	}
	std::cout << "[TextureManager] Number of streaming assets: " << this->streamingAssetCount << std::endl;
}

void TextureManager::instantiateAsTexture(String path, String assetName, bool isStreaming)
{
	sf::Texture* texture = new sf::Texture();
	texture->loadFromFile(path);
	this->textureMap[assetName].push_back(texture);

	if (isStreaming)
	{
		// this->streamTextureList.push_back(texture);
		// Texture will be set at specific index via setStreamTextureAtIndex
	}
	else
	{
		this->baseTextureList.push_back(texture);
	}
}

void TextureManager::initializeStreamTextureList(int size)
{
	this->streamTextureList.resize(size, nullptr);
	std::cout << "[TextureManager] Pre-allocated stream texture list for " << size << " textures" << std::endl;
}

void TextureManager::setStreamTextureAtIndex(int index, sf::Texture* texture)
{
	if (index >= 0 && index < this->streamTextureList.size())
	{
		this->streamTextureList[index] = texture;
	}
}