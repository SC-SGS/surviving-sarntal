//
// Created by Anietta Weckauff on 06.05.24.
//

#ifndef SURVIVING_SARNTAL_WORLD_H
#define SURVIVING_SARNTAL_WORLD_H

#include "Hiker.h"
#include "Inventory.h"
#include "Item.h"
#include "Monster.h"
#include "Mountain.h"
#include "Rock.h"

class World {

  public:
    World();
    void initilizeWorld();

  private:
    Hiker hiker;
    Inventory inventory;
    Monster monster;
    Mountain mountain;
};

#endif // SURVIVING_SARNTAL_WORLD_H
