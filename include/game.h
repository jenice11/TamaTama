#pragma once
#include <array>
#include "textureManager.h"
#include "pet.h"

class Game {
private:
	TextureManager textureManager;
	sf::RenderWindow window;
	Pet pet;
	sf::Font font;
	std::unique_ptr<std::array<sf::Texture, 7>> petTextures; // Different mood textures
	sf::Sprite petSprite;
	sf::Texture heartTexture;
	std::unique_ptr<std::array<std::array<sf::Sprite, 5>, 5>> hearts; // 5 stats, each max 5 hearts
	sf::Text statusTexts[5];
	sf::Text nameAgeText;
	sf::Text moodText;
	sf::RectangleShape buttons[5];
	sf::Text buttonLabels[5];
	const std::string saveFilePath = "Saves/pet.save";
	bool shouldSaveOnExit;

	sf::Clock backgroundUpdateClock;
	const float BACKGROUND_UPDATE_INTERVAL = 1.0f; // Update every second when not focused

	// Death UI elements
	sf::RectangleShape deathBox;
	sf::Text deathTitle;
	sf::Text deathMessage;
	sf::RectangleShape newPetButton;
	sf::Text newPetButtonLabel;

	// Create new pet UI elements
	bool isCreatingNewPet;
	sf::RectangleShape nameInputBox;
	sf::Text nameInputText;
	sf::Text namePromptText;
	std::string inputName;
	bool isInputActive;

	void loadAssets();
	void createNewPet(const std::string& name);
	void updateUI();
	void handleEvents();

public:
	Game();
	void run();
};

