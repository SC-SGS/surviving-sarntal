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

class Renderer : public Singleton<Renderer> {
    friend class Singleton<Renderer>;

  public:
    void draw();

  private:
    Renderer();
    ~Renderer();

    World &world = World::getInstance(); // Pointer to the World object
    void renderEntity(RenderedEntity &entity);
    void renderEntity(RenderedEntity &entity, float rotation);
    static void renderEntity(RenderedEntity &entity, float rotation, Texture2D texture, Rectangle sourceRec);
    void renderBackground();
    ResourceManager &resourceManager = ResourceManager::getInstance();
    Camera2D camera = {0};
    std::unordered_map<std::string, AnimationInformation> animations;

    // Initialize the scrolling speed
    float scrolling_mid = 0;
    float scrolling_fore = 0;

    Texture2D gradient_texture_background{};
    void regenerateGradientTexture();
    void renderRock(RenderedEntity &entity);
    void renderHiker(RenderedEntity &hiker);
    static void drawBackgroundTextureRepeatedly(Texture2D texture2D, float scrolling, float scale, float offsetY);
    void renderMountain(Mountain &mountain, Color topColor = WHITE, Color bottomColor = BLUE) const;
    void renderEntities();
    void animateEntity(RenderedEntity &entity);
    void renderAnimation(RenderedEntity &entity);
    void renderInventory();
    void renderItemSlot(Inventory &inventory, int slotNumber, int startX, int startY);
    void renderHealthBar();
};

#endif // SURVIVING_SARNTAL_RENDERER_H
