#pragma once
#include <array>
#include <SFML/Audio.hpp> 
#include "textureManager.h"
#include "pet.h"
#include "shop.h"

class Game {
private:
	sf::RenderWindow window;

	TextureManager textureManager;
	std::unique_ptr<Pet> pet;
	std::unique_ptr<PetShop> shop;

	sf::Texture backgroundTexture;
	sf::Sprite backgroundSprite;

	sf::Font font;
	sf::Music backgroundMusic;

	std::unique_ptr<std::array<sf::Texture, 7>> petTextures; // Different mood textures
	sf::Sprite petSprite;
	sf::Texture heartTexture;
	std::unique_ptr<std::array<std::array<sf::Sprite, 5>, 5>> hearts; // 5 stats, each max 5 hearts
	std::unique_ptr<std::array<sf::Text, 5>> statusTexts;
	sf::Text nameAgeText;
	sf::Text moodText;
	sf::Text moneyText;
	std::unique_ptr<std::array<sf::RectangleShape, 7>> buttons;
	std::unique_ptr<std::array<sf::Text, 7>> buttonLabels;
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
	bool isFirstLaunch;

	// Inventory UI elements
	bool showingInventory;
	std::vector<sf::RectangleShape> inventoryItemBoxes;
	std::vector<sf::Text> inventoryItemTexts;
	sf::RectangleShape inventoryBackground;
	sf::Text inventoryTitle;
	sf::RectangleShape closeInventoryButton;
	sf::Text closeInventoryText;

	// Shop UI elements
	bool showingShop;
	std::vector<sf::RectangleShape> shopItemBoxes;
	std::vector<sf::Text> shopItemTexts;
	sf::RectangleShape shopBackground;
	sf::Text shopTitle;
	sf::RectangleShape closeShopButton;
	sf::Text closeShopText;

	// Item selection UI elements
	bool showingItemSelection;
	std::string currentSelectionCategory;
	std::vector<sf::RectangleShape> selectionItemBoxes;
	std::vector<sf::Text> selectionItemTexts;
	sf::RectangleShape selectionBackground;
	sf::Text selectionTitle;
	sf::RectangleShape closeSelectionButton;
	sf::Text closeSelectionText;

	void loadAssets();
	void loadGameUI();
	void createNewPet(const std::string& name);

	void showItemsByCategory(const std::string& category);
	void updateSelectionUI(const std::string& category);
	void updateUI();
	void updateInventoryUI();
	void updateShopUI();
	void handleEvents();

public:
	Game();
	void run();
};

