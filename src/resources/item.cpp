#include "Item.h"
#include "Pet.h"
#include <iostream>

Item::Item(const std::string& itemName, int itemValue) :
    name(itemName), value(itemValue), consumed(false) {
}

const std::string& Item::getName() const {
    return name;
}

int Item::getValue() const {
    return value;
}

bool Item::isConsumed() const {
    return consumed;
}

FoodItem::FoodItem(const std::string& foodName, int foodValue, int hungerReduc, int energyB) :
    Item(foodName, foodValue),
    hungerReduction(hungerReduc),
    energyBoost(energyB) {
}

void FoodItem::use(Pet* pet) {
    if (pet) {
        pet->feed(hungerReduction);
        consumed = true;
        std::cout << "Fed " << name << " to pet. Hunger reduced by " << hungerReduction << std::endl;
    }
}

MedicineItem::MedicineItem(const std::string& medName, int medValue, int healthB, int happinessR) :
    Item(medName, medValue),
    healthBoost(healthB),
    happinessReduction(happinessR) {
}

void MedicineItem::use(Pet* pet) {
    if (pet) {
        pet->medicine(healthBoost);
        consumed = true;
        std::cout << "Used " << name << " on pet. Health increased by " << healthBoost << std::endl;
    }
}