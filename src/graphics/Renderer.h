//
// Created by six on 5/7/24.
//

#ifndef SURVIVING_SARNTAL_RENDERER_H
#define SURVIVING_SARNTAL_RENDERER_H

#include "../entities/Hiker.h"
#include "../entities/RenderedEntity.h"
#include "../entities/Rock.h"
#include "../utils/game_constants.h"
#include "ResourceManager.h"
#include "raylib.h"
#include "render_information/RenderInformation.h"

constexpr char WINDOW_NAME[] = "Surviving Sarntal";

class Renderer {
  public:
    Renderer() {
        // Initialize the camera
        camera.position = {0.0f, 0.0f, 300.0f}; // Camera position
        camera.target = {0.0f, 0.0f, 0.0f};     // Camera looking at point
        camera.up = {0.0f, 1.0f, 0.0f};         // Camera up vector (rotation towards target)
        camera.fovy = 45.0f;                    // Camera field-of-view Y
        camera.projection = CAMERA_PERSPECTIVE; // Camera mode type
        regenerateGradientTexture();
    }
    void render(Hiker hiker, RockClass *rocks, int numRocks);

  private:
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
    void renderHiker(const RenderInformation &hikerInfo);
    void animateHiker(RenderInformation &hikerInfo);
    void renderEntity(const RenderInformation &info, float rotation, Texture2D texture, Rectangle sourceRec);
    void renderHiker(RenderInformation &hikerInfo);
    static void drawBackgroundTextureRepeatedly(Texture2D texture2D, float scrolling, float scale, float offsetY);
};

#endif // SURVIVING_SARNTAL_RENDERER_H
