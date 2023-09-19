#include "iostream"
#include <flecs.h>
#include "raylib.h"
#include "entities/mountain.h"

int main() {
    std::cout << "surviving sarntal" << std::endl;

    flecs::world world;

    InitWindow(100, 100, "raylib [core] example - basic window");

    SetTargetFPS(60);

    //auto mountain_entity = world.entity<Mountain>().add<Mountain>();
    auto mountain_entity = world.entity().add<Mountain>();

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Congrats! You created your first window!", 190, 200, 20,
                 LIGHTGRAY);
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
