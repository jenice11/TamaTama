#include <iostream>
#include <fstream>
#include <filesystem>
#include "pet.h"

constexpr int AGE_INTERVAL_MINUTES = 5; // Age up every 5 minutes
constexpr int DEATH_CONDITION_HOURS = 1; // Die after 1 hours of critical condition

Pet::Pet(const std::string& petName) :
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
bool Pet::savePetToFile(const std::string& filename) const {
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
bool Pet::loadPetFromFile(const std::string& filename) {
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

void Pet::update() {
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

void Pet::feed() {
	if (isAlive) {
		hunger = std::max(0, hunger - 30);

		if (isInCriticalHunger) {
			isInCriticalHunger = false;
			criticalHungerStartTime = 0;
			std::cout << "Pet is no longer critically hungry" << std::endl;
		}
	}
}

void Pet::play() {
	if (isAlive && energy > 10) {
		happiness = std::min(100, happiness + 25);
		energy = std::max(0, energy - 10);
		hunger = std::min(100, hunger + 5);
	}
}

void Pet::sleep() {
	if (isAlive) {
		energy = std::min(100, energy + 50);
		hunger = std::min(100, hunger + 15);
	}
}

void Pet::clean() {
	if (isAlive) {
		cleanliness = 100;
		happiness = std::min(100, happiness + 5);
	}
}

void Pet::medicine() {
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

int Pet::getHunger() const { return hunger; }
int Pet::getHappiness() const { return happiness; }
int Pet::getEnergy() const { return energy; }
int Pet::getCleanliness() const { return cleanliness; }
int Pet::getHealth() const { return health; }
int Pet::getAge() const { return age; }
bool Pet::getIsAlive() const { return isAlive; }
std::string Pet::getName() const { return name; }

std::string Pet::getMood() const {
	if (!isAlive) return "Dead";
	if (hunger > 80) return "Hungry";
	if (energy < 20) return "Tired";
	if (cleanliness < 30) return "Dirty";
	if (health < 40) return "Sick";
	if (happiness < 30) return "Sad";
	if (happiness > 80) return "Happy";
	return "Normal";
}
