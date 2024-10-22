//
// Created by six on 5/7/24.
//

#ifndef SURVIVING_SARNTAL_RENDERER_H
#define SURVIVING_SARNTAL_RENDERER_H

#include "../../entities/World.h"
#include "../../menu/MenuEngine.h"
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
             MenuEngine &menuEngine,
             MountainRenderer &mountainRenderer,
             EntityRenderer &entityRenderer,
             HudRenderer &hudRenderer);
    ~Renderer() = default;

    void draw();
    void toggleDebugMode();
    void addShake(float intensity);
    void reset();

  private:
    // Dependencies
    World &world;
    ResourceManager &resourceManager;
    GameConstants &gameConstants;
    MenuEngine &menuEngine;
    // Renderers
    MountainRenderer &mountainRenderer;
    EntityRenderer &entityRenderer;
    HudRenderer &hudRenderer;

    // Attributes
    Camera2D &camera;
    bool debugMode = false;
    Vector2 screenCenter = {};
    float shakeIntensity = 0.0f;

    // Helper functions
    void initCamera();
    void applyRumbleEffect();
    void renderBackground();
    void drawBackgroundTextureRepeatedly(const Texture2D &texture, floatType drawnWidth, floatType offset) const;
    void renderPhysicsTimeStepSize() const;
    void renderShieldTime() const;
};

#endif // SURVIVING_SARNTAL_RENDERER_H
