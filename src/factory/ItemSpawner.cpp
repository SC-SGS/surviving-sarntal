//
// Created by Anietta Weckauff on 14.05.24.
//
#include "ItemSpawner.h"
#include "../entities/Item.h"
#include <chrono>
#include <ctime>

// NOLINTBEGIN
std::random_device dev;
std::mt19937 ItemSpawner::randomEngine(0);
std::uniform_int_distribution<int> ItemSpawner::distribution{0, RAND_MAX};
// NOLINTEND

ItemSpawner::ItemSpawner() = default;

int ItemSpawner::generateRandomNumberGeometric(int p_exp) {
    int rand = RAND_MAX;
    for (int i = 0; i < p_exp; ++i) {
        rand = rand & distribution(randomEngine);
    }
    return std::floor(std::log(RAND_MAX) - std::log(rand));
}

void ItemSpawner::spawnItems(World *world) {
    if (GetTime() - itemSpawnTime > 0) {
        auto itemType = (ItemType)(distribution(randomEngine) % itemCount);

        // TODO figure out camera stuff
        /**float_type x_position = it.world()
                                    .lookup("Camera")
                                    .get_mut<graphics::Camera2DComponent>()
                                    ->target.x +
                                (float)graphics::SCREEN_WIDTH / 2;**/

        // TODO dependent on physics implementation
        auto position = Vector{0, 0};
        /**auto position = Position{
            x_position, physics::getYPosFromX(it.world(), x_position, 0)};**/
        position.y +=
            ITEM_BASE_HEIGHT + (ITEM_MAX_HEIGHT - ITEM_BASE_HEIGHT) * ((float)distribution(randomEngine) / RAND_MAX);
        this->itemSpawnTime =
            static_cast<float>(GetTime() + 3. + (.125 / ITEMS_PER_SECOND * generateRandomNumberGeometric(3)));

        std::cout << "spawning " << itemType << " at " << position.x << "," << position.y << " " << std::endl;

        ItemInformation itemInformation = Item::getItemInformation(itemType);

        Item newItem(itemType, itemInformation, position);

        world->addItem(newItem);
    }
}