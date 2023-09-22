#include "game_logic.h"
#include "../components/player.h"
#include "../components/render_components.h"
#include "../components/vector.h"
#include "../components/mountain.h"
#include "iostream"
#include "raylib.h"
#include "render_systems.h"

void moveKillBar(flecs::iter it, KillBar *killBar) {
    killBar->x += it.delta_time() * KILL_BAR_VELOCITY;
}

void checkPlayerAlive(flecs::iter, Position *position, KillBar *killBar) {
    // TODO multiplayer

    if (position[0].x < killBar->x) {
        std::cout << "Player Dead" << std::endl;
        //exit(0);
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

void moveCamera(flecs::iter it, Position *position, KillBar *killBar, Mountain* mountain) {
    auto camera =
        it.world().lookup("Camera").get_mut<graphics::Camera2DComponent>();
    camera->target.x = (killBar->x)+graphics::SCREEN_WIDTH/2;
    //fix camera to y-coord of player
    //camera->target.y = position[0].y;

    //this abomination of a term is taking the y coordinate of the leftmost mountain vertex that is just barely on the screen
    //and offsetting it by a constant factor of the screen height
    //if there aren't enough points the camera will jerk upwards. with enough points this will do a smooth upwards motion
    camera->target.y = -mountain->getVertex(mountain->getRelevantMountainSection(killBar->x, killBar->x + 0.1).start_index).y - graphics::SCREEN_HEIGHT*0.33;


    std::cout << "Camera position: " << camera->target.y << " mountain left height: " << mountain->getVertex(mountain->getRelevantMountainSection(killBar->x, killBar->x + 0.1).start_index).y <<
        " difference: " << camera->target.y - mountain->getVertex(mountain->getRelevantMountainSection(killBar->x, killBar->x + 0.1).start_index).y<<std::endl;
}


/**
 * Checks if a new chunk should be generated and tells the mountain to generate a new chunk if needed
 * @param it
 * @param mountain
 * @param killBar
 */
void chunkSystem(flecs::iter it, Mountain* mountain, KillBar* killBar){
    float current_left_edge_screen{killBar->x};
    float leftest_point_of_mountain{mountain->getVertex(mountain->getIndexIntervalOfEntireMountain().start_index).x};
    constexpr float CHUNK_DESTROY_BUFFER_CONSTANT{1.0};
    //std::cout << "Position killbar: " << killBar->x << " left point mountain: " << leftest_point_of_mountain << std::endl;
    if(leftest_point_of_mountain < current_left_edge_screen- Mountain::CHUNK_WIDTH - CHUNK_DESTROY_BUFFER_CONSTANT){
        //leftest_point_of_mountain = mountain->getVertex(mountain->getIndexIntervalOfEntireMountain().start_index).x;
        //std::cout << "Position killbar: " << killBar->x << " left point mountain: " << leftest_point_of_mountain << std::endl;
        mountain->generateNewChunk();
        //std::cout << "chunk generated" << std::endl;
    }
}

void initGameLogic(flecs::world &world) {
    world.entity().add<Player>().set<Position>({200.,200.});
    world.set<KillBar>({0.});

    world.system<KillBar>().term_at(1).singleton().iter(moveKillBar);

    world.system<Position, KillBar>()
        .with<Player>()
        .term_at(2)
        .singleton()
        .iter(checkPlayerAlive);

    /*world.system<Position, KillBar>()
        .with<Player>()
        .term_at(2)
        .singleton()
        .iter(debugRenderPlayer);*/

    /*world.system<Position, KillBar>()
        .with<Player>()
        .term_at(2)
        .singleton()
        .iter(moveCamera);*/

    world.system<Mountain, KillBar>().term_at(1).singleton().term_at(2).singleton().iter(chunkSystem);

    world.system<Position, KillBar, Mountain>()
        .with<Player>()
        .term_at(2)
        .singleton()
        .term_at(3).singleton()
        .iter(moveCamera);

}
