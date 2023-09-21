#include "game_logic.h"
#include "../components/particle_state.h"
#include "../components/player.h"
#include "iostream"
#include "raylib.h"

void moveKillBar(flecs::iter it, KillBar *killBar) {
    killBar->x += it.delta_time() * KILL_BAR_VELOCITY;
}

void checkPlayerAlive(flecs::iter, Position *position, KillBar *killBar) {
    // TODO multiplayer

    if (position[0].x < killBar->x) {
        std::cout << "Player Dead" << std::endl;
    }
}

void debugRenderPlayer(flecs::iter, Position *position, KillBar *killBar) {
    Vector2 pos{position[0].x, position[0].y};

    BeginDrawing();
    DrawLine(killBar->x, 0, killBar->x, 1000.0, RED);

    DrawCircleV(pos, 20., RED);
    EndDrawing();
}

void initGameLogic(flecs::world &world) {
    world.entity().add<Player>().set<Position>({200., 200.});
    world.set<KillBar>({0.});

    world.system<KillBar>().term_at(1).singleton().iter(moveKillBar);

    world.system<Position, KillBar>()
        .with<Player>()
        .term_at(2)
        .singleton()
        .iter(checkPlayerAlive);

    world.system<Position, KillBar>().with<Player>().term_at(2).singleton().iter(debugRenderPlayer);
}
