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

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 450;
const int AGE_INTERVAL_MINUTES = 5; // Age up every 5 minutes

// Template for getting/setting data in save file
template<typename T>
class SaveValue {
private:
	T value;

public:
	SaveValue(T defaultValue) : value(defaultValue) {}

	T get() const { return value; }

	void set(T newValue) { value = newValue; }

	void writeTo(std::ofstream& outFile) const {
		outFile << value << std::endl;
	}

	bool readFrom(std::ifstream& inFile) {
		if (inFile >> value) {
			return true;
		}
		return false;
	}
};

class Pet {
private:
	SaveValue<int> hunger;         // 0-100 (0: full, 100: starving)
	SaveValue<int> happiness;      // 0-100 (0: sad, 100: very happy)
	SaveValue<int> energy;         // 0-100 (0: exhausted, 100: energetic)
	SaveValue<int> cleanliness;    // 0-100 (0: dirty, 100: clean)
	SaveValue<int> health;         // 0-100 (0: sick, 100: healthy)
	SaveValue<int> age;            // in days
	SaveValue<bool> isAlive;
	SaveValue<std::string> name;
	std::time_t lastUpdateTime;
	std::time_t lastAgeTime;       // Last time the pet aged
	std::time_t birthTime;

public:
	Pet(const std::string& petName) :
		hunger(20),
		happiness(80),
		energy(100),
		cleanliness(100),
		health(100),
		age(0),
		isAlive(true),
		name(petName) {
		std::time_t currentTime = std::time(nullptr);
		lastUpdateTime = currentTime;
		lastAgeTime = currentTime;
		birthTime = currentTime;
	}

	// Save pet state to file
	bool saveToFile(const std::string& filename) const {
		std::ofstream outFile(filename);
		if (!outFile.is_open()) {
			std::cerr << "Failed to open save file for writing" << std::endl;
			return false;
		}

		hunger.writeTo(outFile);
		happiness.writeTo(outFile);
		energy.writeTo(outFile);
		cleanliness.writeTo(outFile);
		health.writeTo(outFile);
		age.writeTo(outFile);
		isAlive.writeTo(outFile);
		name.writeTo(outFile);

		// Save time values
		outFile << lastUpdateTime << std::endl;
		outFile << lastAgeTime << std::endl;
		outFile << birthTime << std::endl;

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
		if (!hunger.readFrom(inFile) ||
			!happiness.readFrom(inFile) ||
			!energy.readFrom(inFile) ||
			!cleanliness.readFrom(inFile) ||
			!health.readFrom(inFile) ||
			!age.readFrom(inFile) ||
			!isAlive.readFrom(inFile) ||
			!name.readFrom(inFile) ||
			!(inFile >> lastUpdateTime) ||
			!(inFile >> lastAgeTime) ||
			!(inFile >> birthTime)) {

			std::cerr << "Error reading save file" << std::endl;
			return false;
		}

		// Reset last update time to avoid big stat changes
		std::time_t currentTime = std::time(nullptr);

		// Check pet aging when the game is closed
		double minutesPassed = difftime(currentTime, lastAgeTime) / 60.0;
		int daysToAdd = static_cast<int>(minutesPassed / AGE_INTERVAL_MINUTES);

		if (daysToAdd > 0) {
			age.set(age.get() + daysToAdd);
			std::cout << "Pet aged " << daysToAdd << " days while you were away" << std::endl;
			// Recalculate last age time based on passed days
			lastAgeTime = currentTime - static_cast<time_t>(
				(minutesPassed - (daysToAdd * AGE_INTERVAL_MINUTES)) * 60);
		}

		lastUpdateTime = currentTime;

		inFile.close();
		return true;
	}

	void update() {
		std::time_t currentTime = std::time(nullptr);

		// Update stats every minute
		double minutesPassed = difftime(currentTime, lastUpdateTime) / 60.0;

		if (minutesPassed >= 1.0) {
			// Increase hunger
			hunger.set(std::min(100, std::max(0, hunger.get() + static_cast<int>(minutesPassed * 5))));
			// Decrease happiness
			happiness.set(std::min(100, std::max(0, happiness.get() - static_cast<int>(minutesPassed * 3))));
			// Decrease energy
			energy.set(std::min(100, std::max(0, energy.get() - static_cast<int>(minutesPassed * 2))));
			// Decrease cleanliness
			cleanliness.set(std::min(100, std::max(0, cleanliness.get() - static_cast<int>(minutesPassed * 4))));

			// Update health based on other stats
			health.set(100 - ((hunger.get() + (100 - happiness.get()) + (100 - energy.get()) + (100 - cleanliness.get())) / 4));

			if (health.get() <= 0) {
				isAlive.set(false);
			}

			lastUpdateTime = currentTime;
		}

		// Check pet aging for every AGE_INTERVAL_MINUTES
		double ageMinutesPassed = difftime(currentTime, lastAgeTime) / 60.0;

		if (ageMinutesPassed >= AGE_INTERVAL_MINUTES) {
			// Calculate how many days to add when the game was closed
			int daysToAdd = static_cast<int>(ageMinutesPassed / AGE_INTERVAL_MINUTES);
			age.set(age.get() + daysToAdd);

			// Update last age time for remaining minutes
			lastAgeTime = currentTime - static_cast<time_t>(
				(ageMinutesPassed - (daysToAdd * AGE_INTERVAL_MINUTES)) * 60);

			std::cout << "Pet aged to " << age.get() << " days" << std::endl;
		}
	}

	void feed() {
		if (isAlive.get()) {
			hunger.set(std::max(0, hunger.get() - 30));
			energy.set(std::min(100, energy.get() + 5));
		}
	}

	void play() {
		if (isAlive.get() && energy.get() > 10) {
			happiness.set(std::min(100, happiness.get() + 25));
			energy.set(std::max(0, energy.get() - 10));
			hunger.set(std::min(100, hunger.get() + 5));
		}
	}

	void sleep() {
		if (isAlive.get()) {
			energy.set(std::min(100, energy.get() + 50));
			hunger.set(std::min(100, hunger.get() + 15));
		}
	}

	void clean() {
		if (isAlive.get()) {
			cleanliness.set(100);
			happiness.set(std::min(100, happiness.get() + 5));
		}
	}

	void medicine() {
		if (isAlive.get() && health.get() < 80) {
			health.set(std::min(100, health.get() + 20));
			happiness.set(std::max(0, happiness.get() - 5));
		}
	}

	// Getters
	int getHunger() const { return hunger.get(); }
	int getHappiness() const { return happiness.get(); }
	int getEnergy() const { return energy.get(); }
	int getCleanliness() const { return cleanliness.get(); }
	int getHealth() const { return health.get(); }
	int getAge() const { return age.get(); }
	bool getIsAlive() const { return isAlive.get(); }
	std::string getName() const { return name.get(); }

	std::string getMood() const {
		if (!isAlive.get()) return "Dead";
		if (hunger.get() > 80) return "Hungry";
		if (energy.get() < 20) return "Tired";
		if (cleanliness.get() < 30) return "Dirty";
		if (health.get() < 40) return "Sick";
		if (happiness.get() < 30) return "Sad";
		if (happiness.get() > 80) return "Happy";
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

	// Background update timer
	sf::Clock backgroundUpdateClock;
	const float BACKGROUND_UPDATE_INTERVAL = 1.0f; // Update every second when not focused

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

		if (!petTextures[0].loadFromFile("Assets/Textures/nyancat.png")) {
			std::cerr << "Failed to load cat texture!" << std::endl;
			throw "Cannot load nyancat.png";
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
			buttons[i].setPosition(80 + i * 130, 350);

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
	}

	void updateUI() {
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
		if (!pet.getIsAlive()) mood = DEAD;
		else if (pet.getHunger() > 80) mood = HUNGRY;
		else if (pet.getEnergy() < 20) mood = TIRED;
		else if (pet.getCleanliness() < 30) mood = DIRTY;
		else if (pet.getHappiness() < 30) mood = SAD;
		else if (pet.getHappiness() > 80) mood = HAPPY;

		petSprite.setTexture(petTextures[mood]);
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

			if (event.type == sf::Event::MouseButtonPressed &&
				event.mouseButton.button == sf::Mouse::Left) {
				sf::Vector2i mousePos = sf::Mouse::getPosition(window);
				// Check which button was clicked
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

public:
	Game() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
		"Virtual Pet",
		sf::Style::Titlebar | sf::Style::Close),
		pet("Tama"),
		shouldSaveOnExit(true) {
		srand(static_cast<unsigned int>(time(nullptr)));
		loadAssets();

		if (!pet.loadFromFile(saveFilePath)) {
			std::cout << "No save file found, starting with a new pet" << std::endl;
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