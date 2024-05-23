//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "Item.h"
#include <iostream>

void Item::useItem(ItemType itemType, Hiker &hiker) {
    switch (itemType) {
    case KAISERSCHMARRN:
        useKaiserschmarrn(hiker);
        break;
    case COIN:
        useCoin();
        break;
    case DUCK:
        useDuck();
        break;
    default:
        break;
    }
}

void Item::useKaiserschmarrn(Hiker &hiker) {
    int currentHealth = hiker.getHealthPoints();
    hiker.setHealthPoints(std::min(currentHealth + KAISERSCHMARRN_HEALTH_RESTORATION, HIKER_MAX_HEALTH));
    std::cout << "used Kaiserschmarrn" << std::endl;
}

// exact implementation missing
void Item::useCoin() { std::cout << "used coin" << std::endl; }
void Item::useDuck() { std::cout << "used duck" << std::endl; }
