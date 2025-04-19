#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>

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
	sf::RectangleShape statusBars[5];
	sf::Text statusTexts[5];
	sf::Text nameAgeText;
	sf::Text moodText;
	sf::RectangleShape buttons[5];
	sf::Text buttonLabels[5];

	enum PetMood {
		NORMAL = 0,
		HAPPY = 1,
		SAD = 2,
		HUNGRY = 3,
		TIRED = 4,
		DIRTY = 5,
		DEAD = 6
	};

	void loadAssets() {
		font.loadFromFile("Assets/Fonts/arial.ttf"); // Make sure to have a font file


		// TODO other texture mood
		if (!petTextures[0].loadFromFile("Assets/Textures/nyancat2.png")) {
			// Error handling if the cat texture fails to load
			std::cerr << "Failed to load cat texture!" << std::endl;
			throw "Cannot load nyancat.png";
		}

		petSprite.setTexture(petTextures[0]);

		// Set origin to center of sprite
		sf::FloatRect spriteBounds = petSprite.getLocalBounds();
		petSprite.setOrigin(spriteBounds.width / 2.0f, spriteBounds.height / 2.0f);

		// Set position to center of the window
		petSprite.setPosition(800 / 2.0f, 450 / 2.0f);

		// Status bars
		std::string statusNames[5] = { "Hunger", "Happiness", "Energy", "Cleanliness", "Health" };
		sf::Color statusColors[5] = {
			sf::Color(255, 165, 0),  // Orange for hunger
			sf::Color(255, 192, 203), // Pink for happiness
			sf::Color(0, 191, 255),   // Blue for energy
			sf::Color(124, 252, 0),   // Green for cleanliness
			sf::Color(220, 20, 60)    // Red for health
		};

		// Bottom bar 
		/*for (int i = 0; i < 5; i++) {
			statusBars[i].setSize(sf::Vector2f(200, 20));
			statusBars[i].setFillColor(statusColors[i]);
			statusBars[i].setPosition(550, 100 + i * 30);

			statusTexts[i].setFont(font);
			statusTexts[i].setString(statusNames[i]);
			statusTexts[i].setCharacterSize(16);
			statusTexts[i].setFillColor(sf::Color::Black);
			statusTexts[i].setPosition(430, 100 + i * 30);
		}*/

		// Top bar
		float startX = 80;         // Starting X position
		float startY = 20;         // Y position for labels
		float barY = 40;         // Y position for bars
		float spacing = 130;       // Horizontal spacing between each status

		for (int i = 0; i < 5; i++) {
			// Set status text (label)
			statusTexts[i].setFont(font);
			statusTexts[i].setString(statusNames[i]);
			statusTexts[i].setCharacterSize(14);
			statusTexts[i].setFillColor(sf::Color::Black);

			// Center text over the bar
			sf::FloatRect textBounds = statusTexts[i].getLocalBounds();
			float labelX = startX + i * spacing + (100 - textBounds.width) / 2;
			statusTexts[i].setPosition(labelX, startY);

			// Set status bar
			statusBars[i].setSize(sf::Vector2f(100, 16)); // Shorter bar
			statusBars[i].setFillColor(statusColors[i]);
			statusBars[i].setPosition(startX + i * spacing, barY);
		}

		// Name and age text
		nameAgeText.setFont(font);
		nameAgeText.setCharacterSize(20);
		nameAgeText.setFillColor(sf::Color::Black);
		nameAgeText.setPosition(310, 100);

		// Mood text
		moodText.setFont(font);
		moodText.setCharacterSize(20);
		moodText.setFillColor(sf::Color::Black);
		moodText.setPosition(340, 280);

		// Action buttons
		std::string buttonTexts[5] = { "Feed", "Play", "Sleep", "Clean", "Medicine" };
		for (int i = 0; i < 5; i++) {
			buttons[i].setSize(sf::Vector2f(120, 40));
			buttons[i].setFillColor(sf::Color(200, 200, 200));
			buttons[i].setOutlineThickness(2);
			buttons[i].setOutlineColor(sf::Color::Black);
			buttons[i].setPosition(100 + i * 130, 350);

			buttonLabels[i].setFont(font);
			buttonLabels[i].setString(buttonTexts[i]);
			buttonLabels[i].setCharacterSize(16);
			buttonLabels[i].setFillColor(sf::Color::Black);
			// Center text on button
			sf::FloatRect textBounds = buttonLabels[i].getLocalBounds();
			buttonLabels[i].setPosition(
				100 + i * 130 + (120 - textBounds.width) / 2,
				350 + (40 - textBounds.height) / 2 - 5  // -5 for visual centering
			);
		}
	}

	void updateUI() {
		// Update status bars
		statusBars[0].setSize(sf::Vector2f(2 * (100 - pet.getHunger()), 20)); // Invert hunger for display
		statusBars[1].setSize(sf::Vector2f(2 * pet.getHappiness(), 20));
		statusBars[2].setSize(sf::Vector2f(2 * pet.getEnergy(), 20));
		statusBars[3].setSize(sf::Vector2f(2 * pet.getCleanliness(), 20));
		statusBars[4].setSize(sf::Vector2f(2 * pet.getHealth(), 20));

		// Update name and age text
		nameAgeText.setString(pet.getName() + " - Age: " + std::to_string(pet.getAge()) + " days");

		// Update mood text and sprite
		moodText.setString("Mood: " + pet.getMood());

		PetMood mood = NORMAL;
		if (!pet.getIsAlive()) {
			mood = DEAD;
		}
		else if (pet.getHunger() > 80) {
			mood = HUNGRY;
		}
		else if (pet.getEnergy() < 20) {
			mood = TIRED;
		}
		else if (pet.getCleanliness() < 30) {
			mood = DIRTY;
		}
		else if (pet.getHappiness() < 30) {
			mood = SAD;
		}
		else if (pet.getHappiness() > 80) {
			mood = HAPPY;
		}

		petSprite.setTexture(petTextures[mood]);
	}

	void handleEvents() {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}

			if (event.type == sf::Event::MouseButtonPressed) {
				if (event.mouseButton.button == sf::Mouse::Left) {
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
	}

public:
	Game() : window(sf::VideoMode(800, 450), "Virtual Pet"), pet("Tama") {
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

			// Draw pet sprite
			window.draw(petSprite);

			// Draw status bars and texts
			for (int i = 0; i < 5; i++) {
				window.draw(statusTexts[i]);
				window.draw(statusBars[i]);
			}

			// Draw name, age and mood
			window.draw(nameAgeText);
			window.draw(moodText);

			// Draw buttons
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