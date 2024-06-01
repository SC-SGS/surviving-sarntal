//
// Created by six on 5/7/24.
//

#ifndef SURVIVING_SARNTAL_RENDERER_H
#define SURVIVING_SARNTAL_RENDERER_H

#include "../../entities/World.h"
#include "../ResourceManager.h"
#include "raylib.h"
#include "render_information/RenderInformation.h"
#include <cmath>
#include <valarray>

constexpr char WINDOW_NAME[] = "Surviving Sarntal";

class Renderer {
  public:
    explicit Renderer(World *world) : world(world) {
        // TODO get border from world
        float leftBorder = world->getMinX();
        float rightBorder = world->getMaxX();

        // Calculate the center of the camera view based on the borders
        float centerX = (leftBorder + rightBorder) / 2.0f;
        float centerY = 0.0f;

        // Calculate the visible width and height
        float visibleWidth = rightBorder - leftBorder;
        float visibleHeight =
            visibleWidth * (static_cast<float>(GetScreenHeight()) / static_cast<float>(GetScreenWidth()));

        // Distance from the camera to the plane (Z position)
        float cameraDistance = 300.0f; // Adjust as needed

        // Calculate the required field of view (FOV) for the vertical axis
        float fovY = 2.0f * std::atan2(visibleHeight / 2.0f, cameraDistance) * (180.0f / PI);

        // Initialize the camera
        camera.position = Vector3{centerX, centerY, cameraDistance}; // Camera position
        camera.target = Vector3{centerX, centerY, 0.0f};             // Camera looking at point
        camera.up = Vector3{0.0f, 1.0f, 0.0f};                       // Camera up vector (rotation towards target)
        camera.fovy = fovY;                                          // Camera field-of-view Y
        camera.projection = CAMERA_PERSPECTIVE;

        regenerateGradientTexture();
    }
    void draw();

  private:
    World *world; // Pointer to the World object
    void renderEntity(const RenderInformation &info);
    void renderEntity(const RenderInformation &info, float rotation);
    void renderBackground();
    ResourceManager &resourceManager = ResourceManager::getInstance();
    Camera3D camera = {0};

    // Initialize the scrolling speed
    float scrolling_mid = 0;
    float scrolling_fore = 0;

    Texture2D gradient_texture_background{};
    void regenerateGradientTexture();
    void renderRock(const RenderInformation &rockInfo);
    void renderHiker(RenderInformation &hikerInfo);
    void animateHiker(RenderInformation &hikerInfo);
    void renderEntity(const RenderInformation &info, float rotation, Texture2D texture, Rectangle sourceRec);
    static void drawBackgroundTextureRepeatedly(Texture2D texture2D, float scrolling, float scale, float offsetY);
    static void renderMountain(MountainClass &mountain, Color topColor = WHITE, Color bottomColor = BLUE);
    void renderEntities();
};

#endif // SURVIVING_SARNTAL_RENDERER_H
