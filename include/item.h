#pragma once
#include <string>

class Pet;

class Item {
protected:
    std::string name;
    int value;
    bool consumed;

public:
    Item(const std::string& itemName, int itemValue);
    virtual ~Item() = default;

    virtual void use(Pet* pet) = 0;

    const std::string& getName() const;
    int getValue() const;
    bool isConsumed() const;
};

class FoodItem : public Item {
private:
    int hungerReduction;
    int energyBoost;

public:
    FoodItem(const std::string& foodName, int foodValue, int hungerReduc, int energyB);
    void use(Pet* pet) override;
};

class MedicineItem : public Item {
private:
    int healthBoost;
    int happinessReduction;

public:
    MedicineItem(const std::string& medName, int medValue, int healthB, int happinessR);
    void use(Pet* pet) override;
};
