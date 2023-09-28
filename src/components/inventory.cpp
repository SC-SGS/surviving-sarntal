#include "inventory.h"
#include "../systems/physics.h"
#include "particle_state.h"
#include "raylib.h"

std::vector<Sound> ITEM_SOUNDS;

Inventory::Inventory(size_t slot_count)
    : selected_slot(0), slots(slot_count, ItemSlot{ItemClass::NO_ITEM}) {}

Inventory::Inventory() : Inventory(3) {}

size_t Inventory::getSlotCount() const { return slots.size(); }

void Inventory::pickup(ItemClass::Items item_type) {
    slots[selected_slot].item_type = item_type;
}

void Inventory::drop() {
    slots[selected_slot].item_type = ItemClass::Items::NO_ITEM;
}

void Inventory::setItem(size_t slot, ItemClass::Items item_type) {
    slots[slot].item_type = item_type;
}

ItemClass::Items Inventory::getItem(size_t slot) const {
    return slots[slot].item_type;
}

size_t Inventory::getSelectedSlot() const { return selected_slot; }

void Inventory::switchItem(int offset) {
    selected_slot = (selected_slot + offset + getSlotCount()) % getSlotCount();
}

void Inventory::updateInventory(flecs::iter it, InputEntity *input_entities,
                                Inventory *inventory_entities) {
    for (auto i : it) {
        auto player = it.entity(i);

        if (input_entities[i].getAxis(Axis::ITEM_SWITCH) != 0.) {
            inventory_entities[i].switchItem(
                (int)input_entities[i].getAxis(Axis::ITEM_SWITCH));
        }

        if (input_entities[i].getEvent(ITEM_DROP) &&
            inventory_entities->getSelectedItem() !=
                ItemClass::Items::NO_ITEM) {
            inventory_entities[i].drop();
        }
        if (input_entities[i].getEvent(ITEM_USE) &&
            inventory_entities->getSelectedItem() !=
                ItemClass::Items::NO_ITEM) {
            auto item_type =
                ITEM_CLASSES[inventory_entities[i].getSelectedItem()];
            useItem(inventory_entities[i].getSelectedItem(), it.world(),
                    player);

            if (item_type.drop_on_use)
                inventory_entities[i].drop();
        }
        if (player.has<CanCollect>()) {
            auto collectible = it.entity(i).get<CanCollect>()->item_entity;
            if (input_entities[i].getEvent(ITEM_PICK) ||
                (ITEM_CLASSES[collectible.get<Item>()->item_id].auto_collect &&
                 it.entity(i).get<CanCollect>()->distance <
                     0.3 * HIKER_ITEM_COLLECTION_RANGE)) {
                std::cout << "Picking up item: "
                          << it.entity(i).has<CanCollect>() << std::endl;

                physics::playPickupSound();

                auto item = ITEM_CLASSES[collectible.get<Item>()->item_id];
                if (item.use_on_pickup)
                    useItem(collectible.get<Item>()->item_id, it.world(),
                            player);
                else {
                    if (inventory_entities[i].getSelectedItem() !=
                        ItemClass::Items::NO_ITEM)
                        inventory_entities[i].drop();
                    inventory_entities[i].pickup(
                        collectible.get<Item>()->item_id);
                }

                collectible.destruct();
            }
        }
    }
}

void Inventory::checkCanCollect(flecs::iter it, Position *positions,
                                InteractionRadius *radii) {
    for (auto i : it) {
        auto collector_entity = it.entity(i);
        collector_entity.remove<CanCollect>();
        it.world().filter<Position, Item>().iter(
            [&](flecs::iter iter, Position *item_positions, Item *items) {
                for (auto j : iter) {
                    auto collectible_entity = iter.entity(j);
                    auto new_dist = item_positions[j].distanceTo(positions[i]);
                    if (new_dist < radii[i].radius) {
                        if (!collector_entity.has<CanCollect>() ||
                            new_dist <
                                collector_entity.get<CanCollect>()->distance) {
                            collector_entity.set<CanCollect>(
                                {collectible_entity, new_dist});
                        }
                    }
                }
            });
    }
}
ItemClass::Items Inventory::getSelectedItem() const {
    return getItem(getSelectedSlot());
}
void Inventory::useItem(ItemClass::Items item_type, const flecs::world &world,
                        flecs::entity &player) {
    PlaySound(ITEM_SOUNDS[item_type]);

    ITEM_CLASSES[item_type].use(world, player);
}

void Inventory::initItems() {
    for (const auto &i : ITEM_CLASSES)
        ITEM_SOUNDS.push_back(LoadSound(i.audio.c_str()));
}

void ItemClass::useKaiserschmarrn(const flecs::world &world,
                                  flecs::entity &player) {
    auto current_health = player.get_mut<Health>()->hp;
    player.get_mut<Health>()->hp = std::min(
        current_health + KAISERSCHMARRN_HEALTH_RESTORATION, HIKER_MAX_HEALTH);
    std::cout << "lecker Kaiserschmarrn" << std::endl;
}

void ItemClass::useCoin(const flecs::world &world, flecs::entity &player) {
    world.get_mut<AppInfo>()->coin_score += COIN_SCORE;
    std::cout << "used coin" << std::endl;
}

void ItemClass::useDuck(const flecs::world &world, flecs::entity &player) {
    std::cout << "used duck" << std::endl;
    PlaySound(physics::duck_duck_sound);
}
