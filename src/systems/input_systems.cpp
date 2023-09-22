#include "input_systems.h"
#include "flecs.h"

void inputPreupdate(flecs::iter iter, InputEntity *entities) {
    for (auto input : iter)
        entities[input].preUpdate();
}

InputSystems::InputSystems(flecs::world &world) {
    world.module<InputSystems>();

    // add input entity as singleton
    world.add<InputEntity>();
    world.system<InputEntity>().kind(flecs::PreUpdate).iter(inputPreupdate);
}
