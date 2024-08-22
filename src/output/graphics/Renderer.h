//
// Created by six on 5/7/24.
//

#ifndef SURVIVING_SARNTAL_RENDERER_H
#define SURVIVING_SARNTAL_RENDERER_H

#include "../../entities/World.h"
#include "../../spawner/rock_spawning/Polygon.h"
#include "../../spawner/rock_spawning/PolygonGenerator.h"
#include "../../spawner/rock_spawning/PolygonRenderer.h"
#include "../../utilities/Singleton.hpp"
#include "../ResourceManager.h"
#include "raylib.h"
#include "renderers/MountainRenderer.h"
#include <cmath>

constexpr char WINDOW_NAME[] = "Surviving Sarntal";

class Renderer {

  public:
    void draw();
    void setShake(float intensity);
    Renderer(World &world, ResourceManager &resourceManager, Camera2D &camera, MountainRenderer &mountainRenderer,
             GameConstants gameConstants, PolygonRenderer &polygonRenderer);
    ~Renderer() = default;

    void toggleDebugMode();
    void addExplosion(const Vector &position, float radius);

  private:
    // Dependencies
    World &world;
    ResourceManager &resourceManager;
    GameConstants gameConstants;

    // Attributes
    Camera2D &camera;
    std::unordered_map<int, AnimationInformation> animations;
    AnimationInformation &getAnimationInformation(int entityId, AnimationInformation &defaultAnimation);
    std::unordered_map<std::string, int> landmarks;
    const Vector2 screenCenter = {static_cast<float>(GetScreenWidth()) / 2, static_cast<float>(GetScreenHeight()) / 2};
    float shakeIntensity = 0.0f;
    Texture2D gradientTextureBackground{};
    bool debugMode = false;
    const std::unique_ptr<std::list<Rock>> destroyedRocks = std::make_unique<std::list<Rock>>();
    MountainRenderer &mountainRenderer;

    // Initialize the scrolling speed
    floatType scrollingMid = 0;
    floatType scrollingFore = 0;

    // Helper functions
    void loadLandmarks();
    void renderEntity(const RenderedEntity &entity) const;
    void renderEntity(const RenderedEntity &entity, floatType rotation) const;
    void renderEntity(const RenderedEntity &entity, floatType rotation, const Texture2D &texture,
                      Rectangle sourceRec) const;
    void renderBackground();
    void regenerateGradientTexture();
    void renderRock(RenderedEntity &entity) const;
    void renderHiker(const Hiker &hiker);
    void drawBackgroundTextureRepeatedly(const Texture2D &texture2D, floatType scrolling, floatType scale,
                                         floatType offsetY) const;
    void renderEntities();
    void animateEntity(const RenderedEntity &entity);
    void renderAnimation(const RenderedEntity &entity);
    void renderInventory() const;
    void renderItemSlot(const Inventory &inventory, int slotNumber, int startX, int startY) const;
    void renderHealthBar() const;
    void debugRenderEntities();
    void debugRenderRock(RenderedEntity &entity) const;
    void renderScore() const;
    void renderCoinScore() const;
    void renderHUD() const;
    void renderAltimeter() const;
    void renderAltimeterStep(int drawY, int drawAltitude, int fontSize) const;
    void applyRumbleEffect();
    void renderNormalEntities();
    std::list<Rock> &getDestroyedRocks() const;
    void renderWalkingHiker(const Hiker &hiker);
    void renderPolygon();
    PolygonGenerator polygonGenerator{};
    PolygonRenderer &polygonRenderer;
    Vector center = {static_cast<floatType>(graphics::SCREEN_WIDTH) / 2,
                     static_cast<floatType>(graphics::SCREEN_HEIGHT) / 3};
    Polygon polygon = polygonGenerator.generatePolygon(100, 100, center);
};

#endif // SURVIVING_SARNTAL_RENDERER_H
