#pragma once
#include <string>
#include <ctime>
#include <vector>
#include <memory>
#include "Item.h"

enum PetMood { NORMAL, HAPPY, SAD, HUNGRY, TIRED, DIRTY, DEAD };

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

	std::vector<std::unique_ptr<Item>> inventory;

public:
	Pet(const std::string& petName);
	~Pet();

	bool savePetToFile(const std::string& filename) const;
	bool loadPetFromFile(const std::string& filename);

	void update();
	void feed(int amount);
	void play();
	void sleep();
	void clean();
	void medicine(int amount);

	// Inventory management
	void addItemToInventory(Item* newItem);
	bool useItemFromInventory(size_t index);
	const std::vector<std::unique_ptr<Item>>& getInventory() const;

	// Getters
	int getHunger() const;
	int getHappiness() const;
	int getEnergy() const;
	int getCleanliness() const;
	int getHealth() const;
	int getAge() const;
	bool getIsAlive() const;
	std::string getName() const;
	std::string getMood() const;
};