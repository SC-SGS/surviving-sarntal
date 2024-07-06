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
    Renderer(World &world, ResourceManager &resourceManager);
    ~Renderer() = default;

    void toggleDebugMode();

  private:
    // Dependencies
    World &world;
    ResourceManager &resourceManager;

    // Attributes
    Camera2D camera = {0};
    std::unordered_map<std::string, AnimationInformation> animations;
    std::unordered_map<std::string, int> landmarks;
    const Vector2 screenCenter = {static_cast<float>(GetScreenWidth()) / 2, static_cast<float>(GetScreenHeight()) / 2};
    float shakeIntensity = 0.0f;
    Texture2D gradientTextureBackground{};
    bool debugMode = false;

    // Initialize the scrolling speed
    floatType scrollingMid = 0;
    floatType scrollingFore = 0;

    // Helper functions
    void loadLandmarks();
    void renderEntity(RenderedEntity &entity) const;
    void renderEntity(RenderedEntity &entity, floatType rotation) const;
    void renderEntity(RenderedEntity &entity, floatType rotation, const Texture2D &texture, Rectangle sourceRec) const;
    void renderBackground();
    void regenerateGradientTexture();
    void renderRock(RenderedEntity &entity) const;
    void renderHiker(RenderedEntity &hiker) const;
    void drawBackgroundTextureRepeatedly(const Texture2D &texture2D, floatType scrolling, floatType scale,
                                         floatType offsetY) const;
    void renderMountain(const Mountain &mountain, Color topColor = WHITE, Color bottomColor = BLUE) const;
    void renderEntities() const;
    void animateEntity(RenderedEntity &entity) const;
    void renderAnimation(RenderedEntity &entity) const;
    void renderInventory() const;
    void renderItemSlot(const Inventory &inventory, int slotNumber, int startX, int startY) const;
    void renderHealthBar() const;
    void debugRenderEntities() const;
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
    void renderNormalEntities() const;
};

#endif // SURVIVING_SARNTAL_RENDERER_H
