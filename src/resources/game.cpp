#include <iostream>
#include "game.h"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 450;
constexpr int MAX_NAME_LENGTH = 15;

void Game::loadAssets() {
	if (!font.loadFromFile("assets/fonts/arial.ttf")) {
		std::cerr << "Failed to load font!" << std::endl;
		throw "Cannot load arial.tff";
	}

	// Load heart texture
	heartTexture = textureManager.getTexture("assets/textures/heart.png");

	// Load pet textures for different moods
	(*petTextures)[NORMAL] = textureManager.getTexture("assets/textures/normal.png");
	(*petTextures)[HAPPY] = textureManager.getTexture("assets/textures/happy.png");
	(*petTextures)[SAD] = textureManager.getTexture("assets/textures/sad.png");
	(*petTextures)[HUNGRY] = textureManager.getTexture("assets/textures/hungry.png");
	(*petTextures)[TIRED] = textureManager.getTexture("assets/textures/tired.png");
	(*petTextures)[DIRTY] = textureManager.getTexture("assets/textures/dirty.png");
	(*petTextures)[DEAD] = textureManager.getTexture("assets/textures/dead.png");

	// Set initial texture
	petSprite.setTexture((*petTextures)[NORMAL]);
}

void Game::loadGameUI() {
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
	nameAgeText.setPosition(320, 150);

	moodText.setFont(font);
	moodText.setCharacterSize(20);
	moodText.setFillColor(sf::Color::Black);
	moodText.setPosition(340, 290);

	std::string buttonTexts[5] = { "Feed", "Play", "Sleep", "Clean", "Medicine" };

	for (int i = 0; i < 5; i++) {
		(*buttons)[i].setSize(sf::Vector2f(120, 40));
		(*buttons)[i].setFillColor(sf::Color(200, 200, 200));
		(*buttons)[i].setOutlineThickness(2);
		(*buttons)[i].setOutlineColor(sf::Color::Black);
		(*buttons)[i].setPosition(80.f + i * 130.f, 350.f);

		(*buttonLabels)[i].setFont(font);
		(*buttonLabels)[i].setString(buttonTexts[i]);
		(*buttonLabels)[i].setCharacterSize(16);
		(*buttonLabels)[i].setFillColor(sf::Color::Black);
		sf::FloatRect textBounds = (*buttonLabels)[i].getLocalBounds();
		(*buttonLabels)[i].setPosition(
			80 + i * 130 + (120 - textBounds.width) / 2,
			350 + (40 - textBounds.height) / 2 - 5
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

	newPetButton.setSize(sf::Vector2f(200, 40));
	newPetButton.setFillColor(sf::Color(100, 200, 100));
	newPetButton.setOutlineColor(sf::Color::Black);
	newPetButton.setOutlineThickness(2);
	newPetButton.setPosition((WINDOW_WIDTH - 200) / 2.0f, 250);

	newPetButtonLabel.setFont(font);
	newPetButtonLabel.setString("Create New Pet");
	newPetButtonLabel.setCharacterSize(16);
	newPetButtonLabel.setFillColor(sf::Color::Black);
	sf::FloatRect newPetBounds = newPetButtonLabel.getLocalBounds();
	newPetButtonLabel.setPosition(
		(WINDOW_WIDTH - newPetBounds.width) / 2.0f,
		250 + (40 - newPetBounds.height) / 2 - 5
	);

	// New pet input
	namePromptText.setFont(font);
	namePromptText.setString("Enter a name for your new pet:");
	namePromptText.setCharacterSize(18);
	namePromptText.setFillColor(sf::Color::Black);
	sf::FloatRect namePromptBounds = namePromptText.getLocalBounds();
	namePromptText.setPosition(
		(WINDOW_WIDTH - namePromptBounds.width) / 2.0f,
		140
	);

	nameInputBox.setSize(sf::Vector2f(200, 40));
	nameInputBox.setFillColor(sf::Color::White);
	nameInputBox.setOutlineColor(sf::Color::Black);
	nameInputBox.setOutlineThickness(2);
	nameInputBox.setPosition((WINDOW_WIDTH - 200) / 2.0f, 180);

	nameInputText.setFont(font);
	nameInputText.setCharacterSize(16);
	nameInputText.setFillColor(sf::Color::Black);
	nameInputText.setPosition((WINDOW_WIDTH - 180) / 2.0f, 190);
}

void Game::createNewPet(const std::string& name) {
	pet = Pet(name);
	isCreatingNewPet = false;
	isInputActive = false;
	std::cout << "Created new pet named: " << name << std::endl;
}

void Game::updateUI() {
	if (pet.getIsAlive()) {
		int stats[5] = {
			100 - pet.getHunger(),
			pet.getHappiness(),
			pet.getEnergy(),
			pet.getCleanliness(),
			pet.getHealth()
		};

		for (int i = 0; i < 5; i++) {
			int heartsToShow = stats[i] / 20;
			// Heart transparency when empty
			for (int j = 0; j < 5; j++) {
				(*hearts)[i][j].setColor(j < heartsToShow ? sf::Color::White : sf::Color(255, 255, 255, 50));
			}
		}

		nameAgeText.setString(pet.getName() + " - Age: " + std::to_string(pet.getAge()) + " days");
		moodText.setString("Mood: " + pet.getMood());

		PetMood mood = NORMAL;
		if (pet.getHunger() > 80) mood = HUNGRY;
		else if (pet.getEnergy() < 20) mood = TIRED;
		else if (pet.getCleanliness() < 30) mood = DIRTY;
		else if (pet.getHappiness() < 30) mood = SAD;
		else if (pet.getHappiness() > 80) mood = HAPPY;

		petSprite.setTexture((*petTextures)[mood]);
	}
	else {
		std::string petName = pet.getName();
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

	window.draw(petSprite);

	if (pet.getIsAlive()) {
		for (int i = 0; i < 5; i++) {
			window.draw((*statusTexts)[i]);
			for (int j = 0; j < 5; j++)
				window.draw((*hearts)[i][j]);
		}

		window.draw(nameAgeText);
		window.draw(moodText);

		for (int i = 0; i < 5; i++) {
			window.draw((*buttons)[i]);
			window.draw((*buttonLabels)[i]);
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

void Game::handleEvents() {
	sf::Event event;
	while (window.pollEvent(event)) {
		switch (event.type) {
		case sf::Event::Closed:
			// Save the pet's state before closing
			if (shouldSaveOnExit) {
				pet.savePetToFile(saveFilePath);
				std::cout << "Pet state saved" << std::endl;
			}
			window.close();
			break;

		case sf::Event::LostFocus:
			// Reset the background update clock when focus is lost
			backgroundUpdateClock.restart();
			break;

		case sf::Event::TextEntered:
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
			nameInputText.setString(inputName);
			break;

		case sf::Event::MouseButtonPressed:
			if (event.mouseButton.button == sf::Mouse::Left) {
				sf::Vector2i mousePos = sf::Mouse::getPosition(window);
				// Special scenario - Pet Died
				if (!pet.getIsAlive()) {
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
				// Normal scenario
				else {
					for (int i = 0; i < 5; i++) {
						if ((*buttons)[i].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
							switch (i) {
							case 0: pet.feed(); break;
							case 1: pet.play(); break;
							case 2: pet.sleep(); break;
							case 3: pet.clean(); break;
							case 4: pet.medicine(); break;
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
	"Virtual Pet",
	sf::Style::Titlebar | sf::Style::Close),
	pet("Tama kun"),
	shouldSaveOnExit(true),
	isCreatingNewPet(false),
	isInputActive(false) {

	// Initialize smart pointers
	petTextures = std::make_unique<std::array<sf::Texture, 7>>();
	hearts = std::make_unique<std::array<std::array<sf::Sprite, 5>, 5>>();
	statusTexts = std::make_unique<std::array<sf::Text, 5>>();
	buttons = std::make_unique<std::array<sf::RectangleShape, 7>>();
	buttonLabels = std::make_unique<std::array<sf::Text, 7>>();

	srand(static_cast<unsigned int>(time(nullptr)));
	loadAssets();
	loadGameUI();

	if (!pet.loadPetFromFile(saveFilePath)) {
		std::cout << "Starting with a new pet" << std::endl;
	}
	else {
		std::cout << "Pet loaded from save file" << std::endl;
	}

	backgroundUpdateClock.restart();
}

void Game::run() {
	while (window.isOpen()) {
		handleEvents();

		pet.update();

		if (!window.hasFocus() && backgroundUpdateClock.getElapsedTime().asSeconds() >= BACKGROUND_UPDATE_INTERVAL) {
			pet.savePetToFile(saveFilePath);
			backgroundUpdateClock.restart();
		}

		updateUI();
	}
}
