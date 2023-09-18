#include "iostream"
#include <flecs.h>
#include "raylib.h"

int main() {
    std::cout << "surviving sarntal" << std::endl;

    flecs::world w;

    InitWindow(100, 100, "raylib [core] example - basic window");

    SetTargetFPS(60);

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
