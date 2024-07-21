//
// Created by six on 5/7/24.
//

#ifndef SURVIVING_SARNTAL_RENDERER_H
#define SURVIVING_SARNTAL_RENDERER_H

#include "../../entities/World.h"
#include "../../utilities/Singleton.hpp"
#include "../ResourceManager.h"
#include "raylib.h"
#include <cmath>

constexpr char WINDOW_NAME[] = "Surviving Sarntal";

class Renderer {

  public:
    void draw();
    void setShake(float intensity);
    Renderer(World &world, ResourceManager &resourceManager, GameConstants gameConstants);
    ~Renderer() = default;

    void toggleDebugMode();
    void addExplosion(const Vector &position, float radius);

  private:
    // Dependencies
    World &world;
    ResourceManager &resourceManager;
    GameConstants gameConstants;

    // Attributes
    Camera2D camera = {0};
    std::unordered_map<int, AnimationInformation> animations;
    AnimationInformation &getAnimationInformation(int entityId, AnimationInformation &defaultAnimation);
    std::unordered_map<std::string, int> landmarks;
    const Vector2 screenCenter = {static_cast<float>(GetScreenWidth()) / 2, static_cast<float>(GetScreenHeight()) / 2};
    float shakeIntensity = 0.0f;
    Texture2D gradientTextureBackground{};
    bool debugMode = false;
    const std::unique_ptr<std::list<Rock>> destroyedRocks = std::make_unique<std::list<Rock>>();

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
    void renderMountain(const Mountain &mountain, Color topColor = WHITE, Color bottomColor = BLUE) const;
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
    Vector transformPosition(const Vector2 &vector, const Vector2 &offset) const;
    Vector transformPosition(const Vector2 &vector) const;
    floatType transformYCoordinate(floatType yCoordinate) const;
    void renderHUD() const;
    void renderAltimeter() const;
    void renderAltimeterStep(int drawY, int drawAltitude, int fontSize) const;
    int floorToNearest(int number, int placeValue) const;
    void applyRumbleEffect();
    void renderNormalEntities();
    std::list<Rock> &getDestroyedRocks() const;
    void renderWalkingHiker(const Hiker &hiker);
};

#endif // SURVIVING_SARNTAL_RENDERER_H
