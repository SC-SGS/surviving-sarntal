//
// Created by six on 5/7/24.
//

#ifndef SURVIVING_SARNTAL_RENDERER_H
#define SURVIVING_SARNTAL_RENDERER_H

#include "../../entities/World.h"
#include "../../utilities/Singleton.hpp"
#include "../ResourceManager.h"
#include "raylib.h"
#include "renderers/EntityRenderer.h"
#include "renderers/HudRenderer.h"
#include "renderers/MountainRenderer.h"
#include "renderers/PolygonRenderer.h"

constexpr char WINDOW_NAME[] = "Surviving Sarntal";

class Renderer {

  public:
    Renderer(World &world,
             ResourceManager &resourceManager,
             Camera2D &camera,
             GameConstants &gameConstants,
             MountainRenderer &mountainRenderer,
             EntityRenderer &entityRenderer,
             HudRenderer &hudRenderer);
    ~Renderer() = default;

    void draw();
    void toggleDebugMode();
    void setShake(float intensity);
    void reset();

  private:
    // Dependencies
    World &world;
    ResourceManager &resourceManager;
    GameConstants &gameConstants;
    // Renderers
    MountainRenderer &mountainRenderer;
    EntityRenderer &entityRenderer;
    HudRenderer &hudRenderer;

    // Attributes
    Camera2D &camera;
    bool debugMode = false;
    const Vector2 screenCenter = {static_cast<float>(GetScreenWidth()) / 2, static_cast<float>(GetScreenHeight()) / 2};
    float shakeIntensity = 0.0f;
    Texture2D gradientTextureBackground{};

    // Initialize the scrolling speed
    floatType scrollingMid = 0;
    floatType scrollingFore = 0;

    // Helper functions
    void initCamera();
    void regenerateGradientTexture();
    void applyRumbleEffect();
    void renderBackground();
    void drawBackgroundTextureRepeatedly(const Texture2D &texture2D,
                                         floatType scrolling,
                                         floatType scale,
                                         floatType offsetY) const;
};

#endif // SURVIVING_SARNTAL_RENDERER_H
