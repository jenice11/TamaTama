#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 450;

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
		lastUpdateTime = std::time(nullptr);
		birthTime = lastUpdateTime;
	}

	void update() {
		std::time_t currentTime = std::time(nullptr);
		// Update stats every minute
		double minutesPassed = difftime(currentTime, lastUpdateTime) / 60.0;

		if (minutesPassed >= 1.0) {
			// Increase hunger
			hunger += static_cast<int>(minutesPassed * 5);
			// Decrease happiness
			happiness -= static_cast<int>(minutesPassed * 3);
			// Decrease energy
			energy -= static_cast<int>(minutesPassed * 2);
			// Decrease cleanliness
			cleanliness -= static_cast<int>(minutesPassed * 4);

			hunger = std::min(100, std::max(0, hunger));
			happiness = std::min(100, std::max(0, happiness));
			energy = std::min(100, std::max(0, energy));
			cleanliness = std::min(100, std::max(0, cleanliness));

			// Update health based on other stats
			health = 100 - ((hunger + (100 - happiness) + (100 - energy) + (100 - cleanliness)) / 4);

			// Update age (1 day = 10 real minutes for gameplay purposes)
			age = static_cast<int>(difftime(currentTime, birthTime) / (60.0 * 10));

			if (health <= 0) {
				isAlive = false;
			}

			lastUpdateTime = currentTime;
		}
	}

	void feed() {
		if (isAlive) {
			hunger = std::max(0, hunger - 30);
			energy += 5;
			energy = std::min(100, energy);
		}
	}

	void play() {
		if (isAlive && energy > 10) {
			happiness = std::min(100, happiness + 25);
			energy = std::max(0, energy - 10);
			hunger += 5;
			hunger = std::min(100, hunger);
		}
	}

	void sleep() {
		if (isAlive) {
			energy = std::min(100, energy + 50);
			hunger += 15;
			hunger = std::min(100, hunger);
		}
	}

	void clean() {
		if (isAlive) {
			cleanliness = 100;
			happiness += 5;
			happiness = std::min(100, happiness);
		}
	}

	void medicine() {
		if (isAlive && health < 80) {
			health += 20;
			health = std::min(100, health);
			happiness -= 5;
			happiness = std::max(0, happiness);
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
				hearts[i][j].setScale(0.04f, 0.04f); // Adjust based on heart image
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
			100 - pet.getHunger(), // invert hunger
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
			if (event.type == sf::Event::Closed)
				window.close();

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
		pet("Tama") {
		srand(static_cast<unsigned int>(time(nullptr)));
		loadAssets();
	}

	void run() {
		window.setFramerateLimit(60);

		while (window.isOpen()) {
			handleEvents();
			pet.update();
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
		}
	}
};

int main() {
	Game game;
	game.run();
	return 0;
}