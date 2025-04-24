#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <filesystem>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 450;
const int AGE_INTERVAL_MINUTES = 5; // Age up every 5 minutes
const int DEATH_CONDITION_HOURS = 1; // Die after 1 hours of critical condition
const int MAX_NAME_LENGTH = 15;


class Pet {
private:
	int hunger;         // 0-100 (0: full, 100: starving)
	int happiness;      // 0-100 (0: sad, 100: very happy)
	int energy;         // 0-100 (0: exhausted, 100: energetic)
	int cleanliness;    // 0-100 (0: dirty, 100: clean)
	int health;         // 0-100 (0: sick, 100: healthy)
	int age;            // in days
	bool isAlive;
	std::string name;
	std::time_t lastUpdateTime;
	std::time_t lastAgeTime;       // Last time the pet aged
	std::time_t birthTime;

	std::time_t criticalHungerStartTime; // When hunger >=80
	std::time_t criticalHealthStartTime; // When health <=20
	bool isInCriticalHunger;
	bool isInCriticalHealth;

public:
	Pet(const std::string& petName) :
		hunger(20),
		happiness(80),
		energy(100),
		cleanliness(100),
		health(100),
		age(0),
		isAlive(true),
		name(petName),
		isInCriticalHunger(false),
		isInCriticalHealth(false) {
		std::time_t currentTime = std::time(nullptr);
		lastUpdateTime = currentTime;
		lastAgeTime = currentTime;
		birthTime = currentTime;
		criticalHungerStartTime = 0;
		criticalHealthStartTime = 0;
	}

	// Save pet state to file
	bool saveToFile(const std::string& filename) const {
		namespace fs = std::filesystem;

		// Create directory if it doesn't exist
		fs::path savePath = fs::path(filename).parent_path();
		if (!savePath.empty() && !fs::exists(savePath)) {
			try {
				std::cerr << "Save directory not found, creating new directory" << std::endl;
				fs::create_directories(savePath);
			}
			catch (const fs::filesystem_error& e) {
				std::cerr << "Failed to create save directory: " << e.what() << std::endl;
				return false;
			}
		}

		std::ofstream outFile(filename);
		if (!outFile.is_open()) {
			std::cerr << "Failed to open save file for writing" << std::endl;
			return false;
		}

		// Write each value to the file
		outFile << hunger << std::endl;
		outFile << happiness << std::endl;
		outFile << energy << std::endl;
		outFile << cleanliness << std::endl;
		outFile << health << std::endl;
		outFile << age << std::endl;
		outFile << (isAlive ? 1 : 0) << std::endl;
		outFile << name << std::endl;
		outFile << lastUpdateTime << std::endl;
		outFile << lastAgeTime << std::endl;
		outFile << birthTime << std::endl;
		outFile << criticalHungerStartTime << std::endl;
		outFile << criticalHealthStartTime << std::endl;
		outFile << (isInCriticalHunger ? 1 : 0) << std::endl;
		outFile << (isInCriticalHealth ? 1 : 0) << std::endl;

		outFile.close();
		return true;
	}

	// Load pet state from file
	bool loadFromFile(const std::string& filename) {
		std::ifstream inFile(filename);
		if (!inFile.is_open()) {
			std::cerr << "No save file found" << std::endl;
			return false;
		}

		// Read all values
		if (!(inFile >> hunger) ||
			!(inFile >> happiness) ||
			!(inFile >> energy) ||
			!(inFile >> cleanliness) ||
			!(inFile >> health) ||
			!(inFile >> age)) {
			std::cerr << "Error reading save file stats" << std::endl;
			return false;
		}

		int alive;
		if (!(inFile >> alive)) {
			std::cerr << "Error reading alive status" << std::endl;
			return false;
		}
		isAlive = (alive != 0);

		// Skip newline so getline() works correctly
		inFile.ignore();

		// Read name as a line
		if (!std::getline(inFile, name)) {
			std::cerr << "Error reading pet name" << std::endl;
			return false;
		}

		if (!(inFile >> lastUpdateTime) ||
			!(inFile >> lastAgeTime) ||
			!(inFile >> birthTime)) {
			std::cerr << "Error reading time values" << std::endl;
			return false;
		}

		if (!(inFile >> criticalHungerStartTime) ||
			!(inFile >> criticalHealthStartTime)) {
			std::cerr << "Error reading critical condition times" << std::endl;
			criticalHungerStartTime = 0;
			criticalHealthStartTime = 0;
			isInCriticalHunger = false;
			isInCriticalHealth = false;
		}
		else {
			int criticalHunger, criticalHealth;
			if (!(inFile >> criticalHunger) ||
				!(inFile >> criticalHealth)) {
				std::cerr << "Error reading critical condition flags" << std::endl;
				isInCriticalHunger = false;
				isInCriticalHealth = false;
			}
			else {
				isInCriticalHunger = (criticalHunger != 0);
				isInCriticalHealth = (criticalHealth != 0);
			}
		}

		// Reset last update time to avoid big stat changes
		std::time_t currentTime = std::time(nullptr);

		// Check pet aging when the game is closed
		double minutesPassed = difftime(currentTime, lastUpdateTime) / 60.0;
		int daysToAdd = static_cast<int>(minutesPassed / AGE_INTERVAL_MINUTES);

		if (daysToAdd > 0) {
			age += daysToAdd;
			std::cout << "Pet aged " << daysToAdd << " days while you were away" << std::endl;
			// Recalculate last age time based on passed days
			lastAgeTime = currentTime - static_cast<time_t>(
				(minutesPassed - (daysToAdd * AGE_INTERVAL_MINUTES)) * 60);
		}
		// Check death if pet was in critical state when game was closed
		if (isAlive && isInCriticalHunger && criticalHungerStartTime > 0) {
			double hoursHungry = difftime(currentTime, criticalHungerStartTime) / 3600.0;
			if (hoursHungry >= DEATH_CONDITION_HOURS) {
				isAlive = false;
				std::cout << "Your pet died of starvation while you were away" << std::endl;
			}
		}

		if (isAlive && isInCriticalHealth && criticalHealthStartTime > 0) {
			double hoursSick = difftime(currentTime, criticalHealthStartTime) / 3600.0;
			if (hoursSick >= DEATH_CONDITION_HOURS) {
				isAlive = false;
				std::cout << "Your pet died of illness while you were away" << std::endl;
			}
		}

		lastUpdateTime = currentTime;

		inFile.close();
		return true;
	}

	void update() {
		if (!isAlive) return;

		std::time_t currentTime = std::time(nullptr);

		// Update stats every minute
		double minutesPassed = difftime(currentTime, lastUpdateTime) / 60.0;

		if (minutesPassed >= 1.0) {
			// Increase hunger
			hunger = std::min(100, std::max(0, hunger + static_cast<int>(minutesPassed * 5)));
			// Decrease happiness
			happiness = std::min(100, std::max(0, happiness - static_cast<int>(minutesPassed * 3)));
			// Decrease energy
			energy = std::min(100, std::max(0, energy - static_cast<int>(minutesPassed * 2)));
			// Decrease cleanliness
			cleanliness = std::min(100, std::max(0, cleanliness - static_cast<int>(minutesPassed * 4)));

			// Update health based on other stats
			health = 100 - ((hunger + (100 - happiness) + (100 - energy) + (100 - cleanliness)) / 4);

			lastUpdateTime = currentTime;
		}

		// Check if hungry for 24 hours
		if (hunger >= 80) {
			if (!isInCriticalHunger) {
				isInCriticalHunger = true;
				criticalHungerStartTime = currentTime;
				std::cout << "Pet is critically hungry!" << std::endl;
			}
			else {
				double hoursHungry = difftime(currentTime, criticalHungerStartTime) / 3600.0;
				if (hoursHungry >= DEATH_CONDITION_HOURS) {
					isAlive = false;
					std::cout << "Your pet died of starvation after " << DEATH_CONDITION_HOURS << " hours without food" << std::endl;
					return;
				}
			}
		}
		else {
			isInCriticalHunger = false;
			criticalHungerStartTime = 0;
		}

		// Check if sick for 24 hours
		if (health <= 20) {
			if (!isInCriticalHealth) {
				isInCriticalHealth = true;
				criticalHealthStartTime = currentTime;
				std::cout << "Pet is critically sick!" << std::endl;
			}
			else {
				double hoursSick = difftime(currentTime, criticalHealthStartTime) / 3600.0;
				if (hoursSick >= DEATH_CONDITION_HOURS) {
					isAlive = false;
					std::cout << "Your pet died of illness after " << DEATH_CONDITION_HOURS << " hours of being sick" << std::endl;
					return;
				}
			}
		}
		else {
			isInCriticalHealth = false;
			criticalHealthStartTime = 0;
		}

		// Check pet aging for every AGE_INTERVAL_MINUTES
		double ageMinutesPassed = difftime(currentTime, lastAgeTime) / 60.0;

		if (ageMinutesPassed >= AGE_INTERVAL_MINUTES) {
			// Calculate how many days to add when the game was closed
			int daysToAdd = static_cast<int>(ageMinutesPassed / AGE_INTERVAL_MINUTES);
			age += daysToAdd;

			// Update last age time for remaining minutes
			lastAgeTime = currentTime - static_cast<time_t>(
				(ageMinutesPassed - (daysToAdd * AGE_INTERVAL_MINUTES)) * 60);

			std::cout << "Pet aged to " << age << " days" << std::endl;
		}
	}

	void feed() {
		if (isAlive) {
			hunger = std::max(0, hunger - 30);
			energy = std::min(100, energy + 5);

			if (isInCriticalHunger) {
				isInCriticalHunger = false;
				criticalHungerStartTime = 0;
				std::cout << "Pet is no longer critically hungry" << std::endl;
			}
		}
	}

	void play() {
		if (isAlive && energy > 10) {
			happiness = std::min(100, happiness + 25);
			energy = std::max(0, energy - 10);
			hunger = std::min(100, hunger + 5);
		}
	}

	void sleep() {
		if (isAlive) {
			energy = std::min(100, energy + 50);
			hunger = std::min(100, hunger + 15);
		}
	}

	void clean() {
		if (isAlive) {
			cleanliness = 100;
			happiness = std::min(100, happiness + 5);
		}
	}

	void medicine() {
		if (isAlive && health < 80) {
			health = std::min(100, health + 20);
			happiness = std::max(0, happiness - 5);

			// Reset critical health condition if health > 20
			if (health > 20 && isInCriticalHealth) {
				isInCriticalHealth = false;
				criticalHealthStartTime = 0;
				std::cout << "Pet is no longer critically sick" << std::endl;
			}
		}
	}

	// Getters
	int getHunger() const { return hunger; }
	int getHappiness() const { return happiness; }
	int getEnergy() const { return energy; }
	int getCleanliness() const { return cleanliness; }
	int getHealth() const { return health; }
	int getAge() const { return age; }
	bool getIsAlive() const { return isAlive; }
	std::string getName() const { return name; }

	std::string getMood() const {
		if (!isAlive) return "Dead";
		if (hunger > 80) return "Hungry";
		if (energy < 20) return "Tired";
		if (cleanliness < 30) return "Dirty";
		if (health < 40) return "Sick";
		if (happiness < 30) return "Sad";
		if (happiness > 80) return "Happy";
		return "Normal";
	}
};

class Game {
private:
	sf::RenderWindow window;
	Pet pet;
	sf::Font font;
	sf::Texture petTextures[7]; // Different mood textures
	sf::Sprite petSprite;
	sf::Texture heartTexture;
	sf::Sprite hearts[5][5]; // 5 stats, each max 5 hearts
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

	enum PetMood { NORMAL, HAPPY, SAD, HUNGRY, TIRED, DIRTY, DEAD };

	void loadAssets() {
		if (!font.loadFromFile("Assets/Fonts/arial.ttf")) {
			std::cerr << "Failed to load font!" << std::endl;
			throw "Cannot load arial.tff";
		}

		if (!heartTexture.loadFromFile("Assets/Textures/heart.png")) {
			std::cerr << "Failed to load heart texture!" << std::endl;
			throw "Cannot load HeartFull.png";
		}

		if (!petTextures[0].loadFromFile("Assets/Textures/normal.png")) {
			std::cerr << "Failed to load texture!" << std::endl;
			throw "Cannot load normal.png";
		}

		petSprite.setTexture(petTextures[0]);

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
			statusTexts[i].setFont(font);
			statusTexts[i].setString(statusNames[i]);
			statusTexts[i].setCharacterSize(14);
			statusTexts[i].setFillColor(sf::Color::Black);

			// Center text over the bar
			sf::FloatRect textBounds = statusTexts[i].getLocalBounds();
			float labelX = startX + i * spacing + (100 - textBounds.width) / 2;
			statusTexts[i].setPosition(labelX, labelY);

			for (int j = 0; j < 5; j++) {
				hearts[i][j].setTexture(heartTexture);
				hearts[i][j].setScale(0.04f, 0.04f);
				hearts[i][j].setPosition(startX + i * spacing + j * 21, heartsY);
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
			buttons[i].setSize(sf::Vector2f(120, 40));
			buttons[i].setFillColor(sf::Color(200, 200, 200));
			buttons[i].setOutlineThickness(2);
			buttons[i].setOutlineColor(sf::Color::Black);
			buttons[i].setPosition(80.f + i * 130.f, 350.f);

			buttonLabels[i].setFont(font);
			buttonLabels[i].setString(buttonTexts[i]);
			buttonLabels[i].setCharacterSize(16);
			buttonLabels[i].setFillColor(sf::Color::Black);
			sf::FloatRect textBounds = buttonLabels[i].getLocalBounds();
			buttonLabels[i].setPosition(
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

	void createNewPet(const std::string& name) {
		pet = Pet(name);
		isCreatingNewPet = false;
		isInputActive = false;
		std::cout << "Created new pet named: " << name << std::endl;
	}

	void updateUI() {
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
					hearts[i][j].setColor(j < heartsToShow ? sf::Color::White : sf::Color(255, 255, 255, 50));
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

			petSprite.setTexture(petTextures[mood]);
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
			petSprite.setTexture(petTextures[DEAD]);

			if (isCreatingNewPet) {
				nameInputText.setString(inputName + (isInputActive ? "_" : ""));
			}
		}
	}

	void handleEvents() {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				// Save the pet's state before closing
				if (shouldSaveOnExit) {
					pet.saveToFile(saveFilePath);
					std::cout << "Pet state saved" << std::endl;
				}
				window.close();
			}

			if (event.type == sf::Event::LostFocus) {
				// Reset the background update clock when focus is lost
				backgroundUpdateClock.restart();
			}

			if (event.type == sf::Event::TextEntered) {
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
			}

			if (event.type == sf::Event::MouseButtonPressed &&
				event.mouseButton.button == sf::Mouse::Left) {
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
						if (buttons[i].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
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


		}
	}

public:
	Game() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
		"Virtual Pet",
		sf::Style::Titlebar | sf::Style::Close),
		pet("Tama kun"),
		shouldSaveOnExit(true),
		isCreatingNewPet(false),
		isInputActive(false) {
		srand(static_cast<unsigned int>(time(nullptr)));
		loadAssets();

		if (!pet.loadFromFile(saveFilePath)) {
			std::cout << "Starting with a new pet" << std::endl;
		}
		else {
			std::cout << "Pet loaded from save file" << std::endl;
		}

		backgroundUpdateClock.restart();
	}

	void run() {
		window.setFramerateLimit(60);

		while (window.isOpen()) {
			handleEvents();

			pet.update();

			if (!window.hasFocus() && backgroundUpdateClock.getElapsedTime().asSeconds() >= BACKGROUND_UPDATE_INTERVAL) {
				pet.saveToFile(saveFilePath);
				backgroundUpdateClock.restart();
			}

			updateUI();

			window.clear(sf::Color(240, 240, 240));

			window.draw(petSprite);

			if (pet.getIsAlive()) {
				for (int i = 0; i < 5; i++) {
					window.draw(statusTexts[i]);
					for (int j = 0; j < 5; j++)
						window.draw(hearts[i][j]);
				}

				window.draw(nameAgeText);
				window.draw(moodText);

				for (int i = 0; i < 5; i++) {
					window.draw(buttons[i]);
					window.draw(buttonLabels[i]);
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

			if (!window.hasFocus()) {
				window.setFramerateLimit(30);
			}
		}
	}
};

int main() {
	Game game;
	game.run();
	return 0;
}