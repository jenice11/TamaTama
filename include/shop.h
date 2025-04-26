#pragma once
#include <vector>
#include <memory>
#include <iostream>
#include "Pet.h"

class PetShop {
private:
	std::vector<std::unique_ptr<Item>> shopItems;
	int playerMoney;

public:
	PetShop();

	~PetShop();

	void restockShop();

	bool buyItem(size_t index, Pet* pet);

	int getMoney() const;
	void addMoney(int amount);

	const std::vector<std::unique_ptr<Item>>& getShopItems() const;
};