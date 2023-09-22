#include "input_systems.h"
#include "flecs.h"

void inputPreupdate(flecs::iter iter, InputEntity *entities) {
    for (auto input : iter)
        entities[input].updateDevices();
}

InputSystems::InputSystems(flecs::world &world) {
    world.module<InputSystems>();

    world.system<InputEntity>().kind(flecs::PreUpdate).iter(inputPreupdate);
}
