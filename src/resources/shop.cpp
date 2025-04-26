#include "shop.h"

PetShop::PetShop() : playerMoney(100) {
		restockShop();
	}

PetShop::~PetShop() {}

	void PetShop::restockShop() {
		shopItems.clear();

		shopItems.push_back(std::make_unique<FoodItem>("Kibble", 5, 30, 5));
		shopItems.push_back(std::make_unique<FoodItem>("Canned Food", 10, 50, 10));
		shopItems.push_back(std::make_unique<FoodItem>("Treat", 15, 70, 15));

		shopItems.push_back(std::make_unique<MedicineItem>("Pill", 5, 20, 5));
		shopItems.push_back(std::make_unique<MedicineItem>("Injection", 10, 40, 10));
		shopItems.push_back(std::make_unique<MedicineItem>("Golden Pill", 15, 60, 15));
	}

	bool PetShop::buyItem(size_t index, Pet* pet) {
		if (index < 0 || index >= static_cast<int>(shopItems.size())) {
			return false;
		}

		Item* selectedItem = shopItems[index].get();
		if (playerMoney >= selectedItem->getValue()) {
			playerMoney -= selectedItem->getValue();

			Item* newItem = nullptr;

			if (FoodItem* food = dynamic_cast<FoodItem*>(selectedItem)) {
				newItem = new FoodItem(*food);
			}
			else if (MedicineItem* med = dynamic_cast<MedicineItem*>(selectedItem)) {
				newItem = new MedicineItem(*med);
			}

			if (newItem && pet) {
				pet->addItemToInventory(newItem);
				std::cout << "Purchased " << newItem->getName() << " for "
					<< newItem->getValue() << " coins" << std::endl;
				return true;
			}
		}

		return false;
	}

	int PetShop::getMoney() const { return playerMoney; }
	void PetShop::addMoney(int amount) { playerMoney += amount; }

	const std::vector<std::unique_ptr<Item>>& PetShop::getShopItems() const {
		return shopItems;
	}
	