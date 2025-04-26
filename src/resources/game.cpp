#include <iostream>
#include <filesystem>
#include "game.h"
#include "shop.h"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 450;
constexpr int MAX_NAME_LENGTH = 15;

void Game::loadAssets() {
	if (!backgroundMusic.openFromFile("assets/audio/bgm.mp3")) {
		std::cerr << "Failed to load audio!" << std::endl;
		throw "Cannot load bgm";
	}
	if (!font.loadFromFile("assets/fonts/arial.ttf")) {
		std::cerr << "Failed to load font!" << std::endl;
		throw "Cannot load arial.tff";
	}

	backgroundTexture = textureManager.getTexture("assets/textures/background.png");
	heartTexture = textureManager.getTexture("assets/textures/heart.png");

	(*petTextures)[NORMAL] = textureManager.getTexture("assets/textures/normal.png");
	(*petTextures)[HAPPY] = textureManager.getTexture("assets/textures/happy.png");
	(*petTextures)[SAD] = textureManager.getTexture("assets/textures/sad.png");
	(*petTextures)[HUNGRY] = textureManager.getTexture("assets/textures/hungry.png");
	(*petTextures)[TIRED] = textureManager.getTexture("assets/textures/tired.png");
	(*petTextures)[DIRTY] = textureManager.getTexture("assets/textures/dirty.png");
	(*petTextures)[DEAD] = textureManager.getTexture("assets/textures/dead.png");

	// Set initial texture
	petSprite.setTexture((*petTextures)[NORMAL]);
	backgroundSprite.setTexture(backgroundTexture);

}

void Game::loadGameUI() {
	backgroundSprite.setOrigin(backgroundSprite.getLocalBounds().width / 2.0f, backgroundSprite.getLocalBounds().height / 2.0f);
	backgroundSprite.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);

	// Set origin to center of pet sprite
	sf::FloatRect spriteBounds = petSprite.getLocalBounds();
	petSprite.setOrigin(spriteBounds.width / 2.0f, spriteBounds.height / 2.0f);

	// Set position to center of the window
	petSprite.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);

	// Status heart
	std::string statusNames[5] = { "Hunger", "Happiness", "Energy", "Cleanliness", "Health" };
	float startX = 80;
	float spacing = 140;
	float labelY = 20;
	float heartsY = 40;

	for (int i = 0; i < 5; i++) {
		(*statusTexts)[i].setFont(font);
		(*statusTexts)[i].setString(statusNames[i]);
		(*statusTexts)[i].setCharacterSize(14);
		(*statusTexts)[i].setFillColor(sf::Color::Black);

		// Center text over the bar
		sf::FloatRect textBounds = (*statusTexts)[i].getLocalBounds();
		float labelX = startX + i * spacing + (100 - textBounds.width) / 2;
		(*statusTexts)[i].setPosition(labelX, labelY);

		for (int j = 0; j < 5; j++) {
			(*hearts)[i][j].setTexture(heartTexture);
			(*hearts)[i][j].setPosition(startX + i * spacing + j * 21, heartsY);
		}
	}

	nameAgeText.setFont(font);
	nameAgeText.setCharacterSize(20);
	nameAgeText.setFillColor(sf::Color::Black);
	nameAgeText.setPosition(290, 85);

	moodText.setFont(font);
	moodText.setCharacterSize(20);
	moodText.setFillColor(sf::Color::Black);
	moodText.setPosition(340, 330);

	std::string buttonTexts[7] = { "Feed", "Play", "Sleep", "Clean", "Medicine", "Inventory", "Shop" };
	for (int i = 0; i < 7; i++) {
		(*buttons)[i].setSize(sf::Vector2f(95, 40));
		(*buttons)[i].setFillColor(sf::Color(135, 206, 235));
		(*buttons)[i].setOutlineThickness(2);
		(*buttons)[i].setOutlineColor(sf::Color::Black);
		(*buttons)[i].setPosition(40.f + i * 105.f, 380.f);

		(*buttonLabels)[i].setFont(font);
		(*buttonLabels)[i].setString(buttonTexts[i]);
		(*buttonLabels)[i].setCharacterSize(16);
		(*buttonLabels)[i].setFillColor(sf::Color::Black);
		sf::FloatRect textBounds = (*buttonLabels)[i].getLocalBounds();
		(*buttonLabels)[i].setPosition(
			40 + i * 105 + (95 - textBounds.width) / 2,
			380 + (42 - textBounds.height) / 2 - 5
		);
	}

	// Death screen
	deathBox.setSize(sf::Vector2f(400, 200));
	deathBox.setFillColor(sf::Color(255, 255, 255, 230));
	deathBox.setOutlineColor(sf::Color::Black);
	deathBox.setOutlineThickness(2);
	deathBox.setPosition((WINDOW_WIDTH - 400) / 2.0f, 120);

	deathTitle.setFont(font);
	deathTitle.setCharacterSize(18);
	deathTitle.setFillColor(sf::Color::Black);

	deathMessage.setFont(font);
	deathMessage.setCharacterSize(18);
	deathMessage.setFillColor(sf::Color::Black);

	newPetButton.setSize(sf::Vector2f(150, 40));
	newPetButton.setFillColor(sf::Color(100, 200, 100));
	newPetButton.setOutlineColor(sf::Color::Black);
	newPetButton.setOutlineThickness(2);
	newPetButton.setPosition((WINDOW_WIDTH - newPetButton.getLocalBounds().width) / 2.0f, 250);

	newPetButtonLabel.setFont(font);
	newPetButtonLabel.setString("Create New Pet");
	newPetButtonLabel.setCharacterSize(16);
	newPetButtonLabel.setFillColor(sf::Color::Black);
	newPetButtonLabel.setPosition((WINDOW_WIDTH - newPetButtonLabel.getLocalBounds().width) / 2.0f + 15, 260);

	// New pet input
	namePromptText.setFont(font);
	namePromptText.setString("Enter a name for your new pet:");
	namePromptText.setCharacterSize(18);
	namePromptText.setFillColor(sf::Color::Black);
	namePromptText.setPosition((WINDOW_WIDTH - namePromptText.getLocalBounds().width) / 2.0f, 140);

	nameInputBox.setSize(sf::Vector2f(200, 40));
	nameInputBox.setFillColor(sf::Color::White);
	nameInputBox.setOutlineColor(sf::Color::Black);
	nameInputBox.setOutlineThickness(2);
	nameInputBox.setPosition((WINDOW_WIDTH - 200) / 2.0f, 180);

	nameInputText.setFont(font);
	nameInputText.setCharacterSize(16);
	nameInputText.setFillColor(sf::Color::Black);
	nameInputText.setPosition((WINDOW_WIDTH - 180) / 2.0f, 190);

	// Inventory UI setup
	inventoryBackground.setSize(sf::Vector2f(600, 350));
	inventoryBackground.setFillColor(sf::Color(240, 240, 240, 250));
	inventoryBackground.setOutlineColor(sf::Color::Black);
	inventoryBackground.setOutlineThickness(2);
	inventoryBackground.setPosition((WINDOW_WIDTH - inventoryBackground.getLocalBounds().width) / 2.0f, 30);

	inventoryTitle.setFont(font);
	inventoryTitle.setString("Inventory");
	inventoryTitle.setCharacterSize(22);
	inventoryTitle.setFillColor(sf::Color::Black);
	inventoryTitle.setPosition((WINDOW_WIDTH - inventoryTitle.getLocalBounds().width) / 2.0f, 60);

	closeInventoryButton.setSize(sf::Vector2f(100, 40));
	closeInventoryButton.setFillColor(sf::Color(240, 128, 128));
	closeInventoryButton.setOutlineColor(sf::Color::Black);
	closeInventoryButton.setOutlineThickness(2);
	closeInventoryButton.setPosition((WINDOW_WIDTH - closeInventoryButton.getLocalBounds().width) / 2.0f, 360);

	closeInventoryText.setFont(font);
	closeInventoryText.setString("Close");
	closeInventoryText.setCharacterSize(16);
	closeInventoryText.setFillColor(sf::Color::Black);
	closeInventoryText.setPosition((WINDOW_WIDTH - closeInventoryText.getLocalBounds().width) / 2.0f, 370);

	// Shop UI setup
	shopBackground.setSize(sf::Vector2f(600, 350));
	shopBackground.setFillColor(sf::Color(240, 240, 255, 250));
	shopBackground.setOutlineColor(sf::Color::Black);
	shopBackground.setOutlineThickness(2);
	shopBackground.setPosition((WINDOW_WIDTH - shopBackground.getLocalBounds().width) / 2.0f, 30);

	shopTitle.setFont(font);
	shopTitle.setString("Pet Shop");
	shopTitle.setCharacterSize(22);
	shopTitle.setFillColor(sf::Color::Black);
	shopTitle.setPosition((WINDOW_WIDTH - shopTitle.getLocalBounds().width) / 2.0f, 50);

	moneyText.setFont(font);
	moneyText.setCharacterSize(18);
	moneyText.setFillColor(sf::Color::Black);
	moneyText.setPosition(540, 50);

	closeShopButton.setSize(sf::Vector2f(100, 40));
	closeShopButton.setFillColor(sf::Color(240, 128, 128));
	closeShopButton.setOutlineColor(sf::Color::Black);
	closeShopButton.setOutlineThickness(2);
	closeShopButton.setPosition((WINDOW_WIDTH - closeShopButton.getLocalBounds().width) / 2.0f, 360);

	closeShopText.setFont(font);
	closeShopText.setString("Close");
	closeShopText.setCharacterSize(16);
	closeShopText.setFillColor(sf::Color::Black);
	closeShopText.setPosition((WINDOW_WIDTH - closeShopText.getLocalBounds().width) / 2.0f, 370);

	// Item selection UI setup
	selectionBackground.setSize(sf::Vector2f(600, 350));
	selectionBackground.setFillColor(sf::Color(240, 240, 240, 250));
	selectionBackground.setOutlineColor(sf::Color::Black);
	selectionBackground.setOutlineThickness(2);
	selectionBackground.setPosition((WINDOW_WIDTH - selectionBackground.getLocalBounds().width) / 2.0f, 30);

	selectionTitle.setFont(font);
	selectionTitle.setCharacterSize(22);
	selectionTitle.setFillColor(sf::Color::Black);

	closeSelectionButton.setSize(sf::Vector2f(100, 40));
	closeSelectionButton.setFillColor(sf::Color(240, 128, 128));
	closeSelectionButton.setOutlineColor(sf::Color::Black);
	closeSelectionButton.setOutlineThickness(2);
	closeSelectionButton.setPosition((WINDOW_WIDTH - closeSelectionButton.getLocalBounds().width) / 2.0f, 360);

	closeSelectionText.setFont(font);
	closeSelectionText.setString("Close");
	closeSelectionText.setCharacterSize(16);
	closeSelectionText.setFillColor(sf::Color::Black);
	closeSelectionText.setPosition((WINDOW_WIDTH - closeSelectionText.getLocalBounds().width) / 2.0f, 370);
}

void Game::createNewPet(const std::string& name) {
	pet.reset(new Pet(name));
	isCreatingNewPet = false;
	isInputActive = false;
	std::cout << "Created new pet named: " << name << std::endl;
}

void Game::showItemsByCategory(const std::string& category) {
	showingItemSelection = true;
	currentSelectionCategory = category;
	updateSelectionUI(category);
}

void  Game::updateSelectionUI(const std::string& category) {
	// Clear previous selection UI elements
	selectionItemBoxes.clear();
	selectionItemTexts.clear();

	// Set the title based on category
	std::string titleText;
	if (category == "Food") {
		titleText = "Select Food Item";
	}
	else if (category == "Medicine") {
		titleText = "Select Medicine";
	}
	selectionTitle.setString(titleText);
	selectionTitle.setPosition((WINDOW_WIDTH - selectionTitle.getLocalBounds().width) / 2.0f, 60);

	// Get inventory items from pet
	const auto& inventory = pet->getInventory();

	// Filter items by category
	std::vector<size_t> selectionIndex;
	for (size_t i = 0; i < inventory.size(); i++) {
		bool matchesCategory = false;
		if (category == "Food" && dynamic_cast<const FoodItem*>(inventory[i].get())) {
			matchesCategory = true;
		}
		else if (category == "Medicine" && dynamic_cast<const MedicineItem*>(inventory[i].get())) {
			matchesCategory = true;
		}

		if (matchesCategory) {
			selectionIndex.push_back(i);
		}
	}

	float startY = 100;
	float itemHeight = 40;
	float spacing = 10;

	// Create selection item UI elements
	for (size_t i = 0; i < selectionIndex.size(); i++) {
		size_t origIndex = selectionIndex[i];
		sf::RectangleShape itemBox;
		itemBox.setSize(sf::Vector2f(500, itemHeight));
		itemBox.setFillColor(sf::Color(189, 252, 201));
		itemBox.setOutlineColor(sf::Color::Black);
		itemBox.setOutlineThickness(1);
		itemBox.setPosition((WINDOW_WIDTH - 500) / 2.0f, startY + (itemHeight + spacing) * i);
		selectionItemBoxes.push_back(itemBox);

		sf::Text itemText;
		itemText.setFont(font);
		itemText.setString(inventory[origIndex]->getName());
		itemText.setCharacterSize(16);
		itemText.setFillColor(sf::Color::Black);
		itemText.setPosition((WINDOW_WIDTH - 480) / 2.0f, startY + (itemHeight + spacing) * i + 10);

		// Store the original index in the user data
		// We'll use this to retrieve the correct item when selected
		itemText.setString(inventory[origIndex]->getName() + " (Value: " +
			std::to_string(inventory[origIndex]->getValue()) + ")");
		selectionItemTexts.push_back(itemText);
	}

	// If no items of this category, show message
	if (selectionIndex.empty()) {
		sf::Text emptyText;
		emptyText.setFont(font);
		emptyText.setString("No " + category + " items in your inventory.");
		emptyText.setCharacterSize(18);
		emptyText.setFillColor(sf::Color::Black);
		emptyText.setPosition(
			(WINDOW_WIDTH - emptyText.getLocalBounds().width) / 2.0f,
			200
		);
		selectionItemTexts.push_back(emptyText);
	}
}

void Game::updateUI() {
	if (isFirstLaunch) {
		window.clear(sf::Color(240, 240, 240));
		window.draw(deathBox);
		window.draw(namePromptText);
		window.draw(nameInputBox);
		window.draw(nameInputText);
		window.draw(newPetButton);
		window.draw(newPetButtonLabel);
		window.display();
		return;
	}

	if (pet->getIsAlive()) {
		int stats[5] = {
			100 - pet->getHunger(),
			pet->getHappiness(),
			pet->getEnergy(),
			pet->getCleanliness(),
			pet->getHealth()
		};

		for (int i = 0; i < 5; i++) {
			int heartsToShow = stats[i] / 20;
			// Heart transparency when empty
			for (int j = 0; j < 5; j++) {
				(*hearts)[i][j].setColor(j < heartsToShow ? sf::Color::White : sf::Color(255, 255, 255, 50));
			}
		}

		nameAgeText.setString(pet->getName() + " - Age: " + std::to_string(pet->getAge()) + " days");
		moodText.setString("Mood: " + pet->getMood());

		PetMood mood = NORMAL;
		if (pet->getHunger() > 80) mood = HUNGRY;
		else if (pet->getEnergy() < 20) mood = TIRED;
		else if (pet->getCleanliness() < 30) mood = DIRTY;
		else if (pet->getHappiness() < 30) mood = SAD;
		else if (pet->getHappiness() > 80) mood = HAPPY;

		petSprite.setTexture((*petTextures)[mood]);

		// Update inventory UI if showing
		if (showingInventory) {
			updateInventoryUI();
		}

		// Update shop UI if showing
		if (showingShop) {
			updateShopUI();
		}
	}
	else {
		std::string petName = pet->getName();
		deathTitle.setString("Your " + petName + " died.");
		deathMessage.setString("Would you like to foster a new pet?");

		// Center the message
		sf::FloatRect textBounds1 = deathTitle.getLocalBounds();
		sf::FloatRect textBounds2 = deathMessage.getLocalBounds();

		deathTitle.setPosition((WINDOW_WIDTH - textBounds1.width) / 2.0f, 150);
		deathMessage.setPosition((WINDOW_WIDTH - textBounds2.width) / 2.0f, 190);

		// Set sprite to dead texture
		petSprite.setTexture((*petTextures)[DEAD]);

		if (isCreatingNewPet) {
			nameInputText.setString(inputName + (isInputActive ? "_" : ""));
		}
	}

	window.clear(sf::Color(240, 240, 240));
	window.draw(backgroundSprite);

	window.draw(petSprite);

	if (pet->getIsAlive()) {
		if (showingInventory) {
			// Draw inventory UI
			window.draw(inventoryBackground);
			window.draw(inventoryTitle);

			for (const auto& box : inventoryItemBoxes) {
				window.draw(box);
			}

			for (const auto& text : inventoryItemTexts) {
				window.draw(text);
			}

			window.draw(closeInventoryButton);
			window.draw(closeInventoryText);
		}
		else if (showingShop) {
			// Draw shop UI
			window.draw(shopBackground);
			window.draw(shopTitle);

			for (const auto& box : shopItemBoxes) {
				window.draw(box);
			}

			for (const auto& text : shopItemTexts) {
				window.draw(text);
			}

			window.draw(moneyText);
			window.draw(closeShopButton);
			window.draw(closeShopText);
		}
		else if (showingItemSelection) {
			// Draw item selection UI
			window.draw(selectionBackground);
			window.draw(selectionTitle);

			for (const auto& box : selectionItemBoxes) {
				window.draw(box);
			}

			for (const auto& text : selectionItemTexts) {
				window.draw(text);
			}

			window.draw(closeSelectionButton);
			window.draw(closeSelectionText);
		}
		else {
			// Draw main game UI
			for (int i = 0; i < 5; i++) {
				window.draw((*statusTexts)[i]);
				for (int j = 0; j < 5; j++)
					window.draw((*hearts)[i][j]);
			}

			window.draw(nameAgeText);
			window.draw(moodText);

			for (int i = 0; i < 7; i++) {
				window.draw((*buttons)[i]);
				window.draw((*buttonLabels)[i]);
			}
		}
	}
	else {
		if (!isCreatingNewPet) {
			// Draw death message and new pet button
			window.draw(deathBox);
			window.draw(deathTitle);
			window.draw(deathMessage);
			window.draw(newPetButton);
			window.draw(newPetButtonLabel);
		}
		else {
			// Draw new pet creation UI
			window.draw(deathBox);
			window.draw(namePromptText);
			window.draw(nameInputBox);
			window.draw(nameInputText);
			window.draw(newPetButton);
			window.draw(newPetButtonLabel);
		}
	}

	window.display();
}

void Game::updateInventoryUI() {
	// Clear previous inventory UI elements
	inventoryItemBoxes.clear();
	inventoryItemTexts.clear();

	// Get inventory items from pet
	const auto& inventory = pet->getInventory();

	float startY = 100;
	float itemHeight = 40;
	float spacing = 10;

	// Create inventory item UI elements
	for (size_t i = 0; i < inventory.size(); i++) {
		sf::RectangleShape itemBox;
		itemBox.setSize(sf::Vector2f(500, itemHeight));
		itemBox.setFillColor(sf::Color(220, 220, 220));
		itemBox.setOutlineColor(sf::Color::Black);
		itemBox.setOutlineThickness(1);
		itemBox.setPosition((WINDOW_WIDTH - 500) / 2.0f, startY + (itemHeight + spacing) * i);
		inventoryItemBoxes.push_back(itemBox);

		sf::Text itemText;
		itemText.setFont(font);

		// Get item type string
		std::string itemTypeStr;
		if (dynamic_cast<const FoodItem*>(inventory[i].get()))
			itemTypeStr = "[Food]";
		else if (dynamic_cast<const MedicineItem*>(inventory[i].get()))
			itemTypeStr = "[Medicine]";

		itemText.setString(itemTypeStr + " " + inventory[i]->getName() + " (Value: " +
			std::to_string(inventory[i]->getValue()) + ")");
		itemText.setCharacterSize(16);
		itemText.setFillColor(sf::Color::Black);
		itemText.setPosition((WINDOW_WIDTH - 480) / 2.0f, startY + (itemHeight + spacing) * i + 10);
		inventoryItemTexts.push_back(itemText);
	}

	// If inventory is empty, show message
	if (inventory.empty()) {
		sf::Text emptyText;
		emptyText.setFont(font);
		emptyText.setString("Your inventory is empty.");
		emptyText.setCharacterSize(18);
		emptyText.setFillColor(sf::Color::Black);
		emptyText.setPosition(
			(WINDOW_WIDTH - emptyText.getLocalBounds().width) / 2.0f,
			200
		);
		inventoryItemTexts.push_back(emptyText);
	}
}

void Game::updateShopUI() {
	// Clear previous shop UI elements
	shopItemBoxes.clear();
	shopItemTexts.clear();

	// Get shop items and group them by category
	const auto& shopItems = shop->getShopItems();
	std::map<std::string, std::vector<std::pair<size_t, const Item*>>> categorizedItems;

	for (size_t i = 0; i < shopItems.size(); i++) {
		std::string category;
		if (dynamic_cast<const FoodItem*>(shopItems[i].get()))
			category = "Food";
		else if (dynamic_cast<const MedicineItem*>(shopItems[i].get()))
			category = "Medicine";
		else
			category = "Other";

		categorizedItems[category].push_back({ i, shopItems[i].get() });
	}

	// UI layout parameters
	float startY = 100;
	float itemHeight = 40;
	float spacing = 10;
	float columnWidth = 210;
	float categorySpacing = 20;
	float leftX = (WINDOW_WIDTH - (2 * columnWidth) - 40) / 2.0f;
	float rightX = leftX + columnWidth + 30;

	// Store mapping of UI box index to shop item index for click handling
	std::vector<size_t> boxToItemIndex;

	moneyText.setString("Money: " + std::to_string(shop->getMoney()) + " coins");

	auto createCategoryHeader = [&](const std::string& category, float x, float y) {
		sf::Text categoryText;
		categoryText.setFont(font);
		categoryText.setString(category + ":");
		categoryText.setCharacterSize(18);
		categoryText.setStyle(sf::Text::Bold);
		categoryText.setFillColor(sf::Color::Black);
		categoryText.setPosition(x, y);
		shopItemTexts.push_back(categoryText);
		return y + 25; // Return the Y position after the header
		};

	// Determine left column contents
	float leftCurrentY = startY;
	if (categorizedItems.find("Food") != categorizedItems.end() && !categorizedItems["Food"].empty()) {
		leftCurrentY = createCategoryHeader("Food", leftX, leftCurrentY);

		for (const auto& [itemIndex, item] : categorizedItems["Food"]) {
			sf::RectangleShape itemBox;
			itemBox.setSize(sf::Vector2f(columnWidth, itemHeight));
			itemBox.setFillColor(sf::Color(224, 255, 255));
			itemBox.setOutlineColor(sf::Color::Black);
			itemBox.setOutlineThickness(1);
			itemBox.setPosition(leftX, leftCurrentY);
			shopItemBoxes.push_back(itemBox);
			boxToItemIndex.push_back(itemIndex);

			// Create item text
			sf::Text itemText;
			itemText.setFont(font);
			itemText.setString(item->getName() + ": " +
				std::to_string(item->getValue()) + " coins");
			itemText.setCharacterSize(16);
			itemText.setFillColor(sf::Color::Black);
			itemText.setPosition(leftX + 10, leftCurrentY + 10);
			shopItemTexts.push_back(itemText);

			leftCurrentY += itemHeight + spacing;
		}

		leftCurrentY += categorySpacing - spacing;
	}

	float rightCurrentY = startY;

	if (categorizedItems.find("Medicine") != categorizedItems.end() && !categorizedItems["Medicine"].empty()) {
		rightCurrentY = createCategoryHeader("Medicine", rightX, rightCurrentY);

		for (const auto& [itemIndex, item] : categorizedItems["Medicine"]) {
			sf::RectangleShape itemBox;
			itemBox.setSize(sf::Vector2f(columnWidth, itemHeight));
			itemBox.setFillColor(sf::Color(224, 224, 255));
			itemBox.setOutlineColor(sf::Color::Black);
			itemBox.setOutlineThickness(1);
			itemBox.setPosition(rightX, rightCurrentY);
			shopItemBoxes.push_back(itemBox);
			boxToItemIndex.push_back(itemIndex);

			sf::Text itemText;
			itemText.setFont(font);
			itemText.setString(item->getName() + ": " +
				std::to_string(item->getValue()) + " coins");
			itemText.setCharacterSize(16);
			itemText.setFillColor(sf::Color::Black);
			itemText.setPosition(rightX + 10, rightCurrentY + 10);
			shopItemTexts.push_back(itemText);

			rightCurrentY += itemHeight + spacing;
		}
	}
}

void Game::handleEvents() {
	sf::Event event;
	while (window.pollEvent(event)) {
		switch (event.type) {
		case sf::Event::Closed:
			// Save the pet's state before closing
			if (shouldSaveOnExit) {
				pet->savePetToFile(saveFilePath);
				std::cout << "Pet state saved" << std::endl;
			}
			window.close();
			break;

		case sf::Event::LostFocus:
			// Reset the background update clock when focus is lost
			backgroundUpdateClock.restart();
			break;

		case sf::Event::TextEntered:
			if ((isCreatingNewPet || isFirstLaunch) && isInputActive) {
				if (event.text.unicode == 8 && !inputName.empty()) {
					inputName.pop_back();
				}
				// Only add character if has valid character
				else if (event.text.unicode >= 32 && event.text.unicode < 128) {
					std::string tempInput = inputName + static_cast<char>(event.text.unicode);
					nameInputText.setString(tempInput);
					sf::FloatRect textBounds = nameInputText.getLocalBounds();
					// Only add the character if it fits inside box
					if (textBounds.width <= nameInputBox.getSize().x - 20) {
						inputName = tempInput;
					}
				}
				nameInputText.setString(inputName + (isInputActive ? "_" : ""));
			}
			break;

		case sf::Event::MouseButtonPressed:
			if (event.mouseButton.button == sf::Mouse::Left) {
				sf::Vector2i mousePos = sf::Mouse::getPosition(window);

				if (isFirstLaunch) {
					if (nameInputBox.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
						isInputActive = true;
					}
					else {
						isInputActive = false;
					}

					if (newPetButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
						if (!inputName.empty()) {
							createNewPet(inputName);
							isFirstLaunch = false;
							shop.reset(new PetShop());
						}
					}
				}
				// Special scenario - Pet Died
				else if (!pet->getIsAlive()) {
					if (!isCreatingNewPet) {
						if (newPetButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
							isCreatingNewPet = true;
							inputName = "";
							isInputActive = true;
						}
					}
					else {
						if (nameInputBox.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
							isInputActive = true;
						}
						else {
							isInputActive = false;
						}
						if (newPetButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
							if (!inputName.empty()) {
								createNewPet(inputName);
							}
						}
					}
				}
				// Showing item selection
				else if (showingItemSelection) {
					// Check if close button is clicked
					if (closeSelectionButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
						showingItemSelection = false;
					}

					// Check if an item is clicked
					const auto& inventory = pet->getInventory();
					std::vector<size_t> filteredIndices;

					// Filter items by category
					for (size_t i = 0; i < inventory.size(); i++) {
						bool matchesCategory = false;
						if (currentSelectionCategory == "Food" && dynamic_cast<const FoodItem*>(inventory[i].get())) {
							matchesCategory = true;
						}
						else if (currentSelectionCategory == "Medicine" && dynamic_cast<const MedicineItem*>(inventory[i].get())) {
							matchesCategory = true;
						}

						if (matchesCategory) {
							filteredIndices.push_back(i);
						}
					}

					for (size_t i = 0; i < selectionItemBoxes.size() && i < filteredIndices.size(); i++) {
						if (selectionItemBoxes[i].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
							// Use the item
							pet->useItemFromInventory(filteredIndices[i]);
							// Update the selection UI
							updateSelectionUI(currentSelectionCategory);
							break;
						}
					}
				}
				else if (showingInventory) {
					// Check if close button is clicked
					if (closeInventoryButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
						showingInventory = false;
					}

					// Check if an inventory item is clicked
					for (size_t i = 0; i < inventoryItemBoxes.size(); i++) {
						if (inventoryItemBoxes[i].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
							pet->useItemFromInventory(i);
							updateInventoryUI();
							break;
						}
					}
				}
				else if (showingShop) {
					if (closeShopButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
						showingShop = false;
					}

					for (size_t i = 0; i < shopItemBoxes.size(); i++) {
						if (shopItemBoxes[i].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
							// Try to buy the item
							shop->buyItem(i, pet.get());
							// Update the shop UI
							updateShopUI();
							break;
						}
					}
				}
				// Normal scenario
				else {
					for (int i = 0; i < 7; i++) {
						if ((*buttons)[i].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
							switch (i) {
							case 0:
								showItemsByCategory("Food");
								break;
							case 1: pet->play(); break;
							case 2: pet->sleep(); break;
							case 3: pet->clean(); break;
							case 4:
								showItemsByCategory("Medicine");
								break;
							case 5:
								showingInventory = true;
								updateInventoryUI();
								break;
							case 6:
								showingShop = true;
								updateShopUI();
								break;
							}
						}
					}
				}
			}
			break;
		}
	}
}

Game::Game() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
	"Tama Tama",
	sf::Style::Titlebar | sf::Style::Close),
	shouldSaveOnExit(true),
	isCreatingNewPet(false),
	isInputActive(false),
	isFirstLaunch(false),
	showingInventory(false),
	showingShop(false),
	showingItemSelection(false),
	currentSelectionCategory("") {

	// Initialize
	petTextures = std::make_unique<std::array<sf::Texture, 7>>();
	hearts = std::make_unique<std::array<std::array<sf::Sprite, 5>, 5>>();
	statusTexts = std::make_unique<std::array<sf::Text, 5>>();
	buttons = std::make_unique<std::array<sf::RectangleShape, 7>>();
	buttonLabels = std::make_unique<std::array<sf::Text, 7>>();

	pet = std::make_unique<Pet>("Tama kun");
	shop = std::make_unique<PetShop>();

	srand(static_cast<unsigned int>(time(nullptr)));
	loadAssets();
	loadGameUI();

	bool saveFileExists = false;

	try {
		std::filesystem::path savePath(saveFilePath);
		saveFileExists = std::filesystem::exists(savePath);
	}
	catch (const std::exception& e) {
		std::cerr << "Error checking for save file: " << e.what() << std::endl;
		saveFileExists = false;
	}

	if (!saveFileExists) {
		std::cout << "No save file found at " << saveFilePath << ". Starting with pet creation." << std::endl;
		isFirstLaunch = true;
		isInputActive = true;
		inputName = "";

		namePromptText.setString("Welcome! Name your new pet:");
		newPetButtonLabel.setString("Start Game");
		nameInputText.setString("_");
	}
	else {
		if (pet->loadPetFromFile(saveFilePath)) {
			std::cout << "Pet loaded from save file: " << saveFilePath << std::endl;
		}
		else {
			std::cout << "Save file exists but could not be loaded. Starting with pet creation." << std::endl;
			isFirstLaunch = true;
			isInputActive = true;
			inputName = "";

			namePromptText.setString("Welcome! Name your new pet:");
			newPetButtonLabel.setString("Start Game");
			nameInputText.setString("_");
		}
	}

	backgroundUpdateClock.restart();
}

void Game::run() {
	backgroundMusic.setLoop(true);
	backgroundMusic.play();

	while (window.isOpen()) {
		handleEvents();

		pet->update();

		if (!window.hasFocus() && backgroundUpdateClock.getElapsedTime().asSeconds() >= BACKGROUND_UPDATE_INTERVAL) {
			pet->savePetToFile(saveFilePath);
			backgroundUpdateClock.restart();
		}

		updateUI();
	}
}
