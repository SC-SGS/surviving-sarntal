#include "game_logic.h"
#include "../components/particle_state.h"
#include "../components/player.h"
#include "../components/render_components.h"
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

// TODO: remove this if not needed
void debugRenderPlayer(flecs::iter it, Position *position, KillBar *killBar) {
    Vector2 pos{position[0].x, position[0].y};

    BeginDrawing();
    auto camera =
        it.world().lookup("Camera").get<graphics::Camera2DComponent>();
    BeginMode2D(*camera);
    Vector2 start{killBar->x, 0.};
    Vector2 stop{killBar->x, 1000.};

    DrawLineV(start, stop, RED);

    DrawCircleV(pos, 20., RED);
    EndMode2D();
    EndDrawing();
}

void moveCamera(flecs::iter it, Position *position, KillBar *killBar) {
    auto camera =
        it.world().lookup("Camera").get_mut<graphics::Camera2DComponent>();
    camera->target.x = killBar->x;
    camera->target.y = position[0].y;
}

void initGameLogic(flecs::world &world) {
    world.entity()
        .add<Player>()
        .set<Position>({200., 200.})
        .set<Velocity>({0., 0.})
        .set<PlayerMovement>({PlayerMovement::MovementState::IDLE});
    world.set<KillBar>({0.});

    /* world.system<KillBar>().term_at(1).singleton().iter(moveKillBar);

    world.system<Position, KillBar>()
        .with<Player>()
        .term_at(2)
        .singleton()
        .iter(checkPlayerAlive);

    world.system<Position, KillBar>()
        .with<Player>()
        .term_at(2)
        .singleton()
        .iter(debugRenderPlayer);

    world.system<Position, KillBar>()
        .with<Player>()
        .term_at(2)
        .singleton()
        .iter(moveCamera);*/
}
