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
#include <valarray>

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
    Texture2D gradient_texture_background{};
    bool debugMode = false;

    // Initialize the scrolling speed
    floatType scrolling_mid = 0;
    floatType scrolling_fore = 0;

    // Helper functions
    void loadLandmarks();
    void renderEntity(RenderedEntity &entity);
    void renderEntity(RenderedEntity &entity, floatType rotation);
    void renderEntity(RenderedEntity &entity, floatType rotation, Texture2D texture, Rectangle sourceRec) const;
    void renderBackground();
    void regenerateGradientTexture();
    void renderRock(RenderedEntity &entity);
    void renderHiker(RenderedEntity &hiker);
    void drawBackgroundTextureRepeatedly(Texture2D texture2D, floatType scrolling, floatType scale, floatType offsetY);
    void renderMountain(Mountain &mountain, Color topColor = WHITE, Color bottomColor = BLUE) const;
    void renderEntities();
    void animateEntity(RenderedEntity &entity);
    void renderAnimation(RenderedEntity &entity);
    void renderInventory();
    void renderItemSlot(Inventory &inventory, int slotNumber, int startX, int startY);
    void renderHealthBar();
    void debugRenderEntities();
    void debugRenderRock(RenderedEntity &entity);
    void renderScore();
    void renderCoinScore();
    void renderHUD();
    void renderAltimeter();
    void renderAltimeterStep(int drawY, int drawAltitude, int fontSize);
    int floorToNearest(int number, int placeValue);
    void applyRumbleEffect();
    void renderNormalEntities();
};

#endif // SURVIVING_SARNTAL_RENDERER_H
