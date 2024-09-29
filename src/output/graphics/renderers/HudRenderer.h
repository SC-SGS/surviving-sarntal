//
// Created by six on 9/23/24.
//

#ifndef SURVIVING_SARNTAL_HUDRENDERER_H
#define SURVIVING_SARNTAL_HUDRENDERER_H

#include "../../../entities/World.h"
#include "PolygonRenderer.h"
class HudRenderer {
  public:
    explicit HudRenderer(World &world,
                         Camera2D &camera,
                         GameConstants &gameConstants,
                         ResourceManager &resourceManager);

    void renderHud(bool newDebugMode);
    void reset();

  private:
    // Dependencies
    World &world;
    ResourceManager &resourceManager;
    GameConstants &gameConstants;
    Camera2D &camera;

    Texture2D scoreTextureX{};
    Texture2D scoreTextureY{};
    bool debugMode = false;
    std::unordered_map<std::string, int> landmarks;

    void loadLandmarks();
    void renderAltimeter() const;
    void renderAltimeterStep(int drawY, int drawAltitude, int fontSize) const;
    void renderInventory() const;
    void renderItemSlot(const Inventory &inventory, int slotNumber, int startX, int startY) const;
    void renderHealthBar() const;
    void renderScore() const;
    void drawScoreTexture(const Texture2D &texture, floatType widthOffset, floatType uiMarginMultiplier) const;
    void drawScoreText(const char *scoreText, int textXOffset) const;
    void renderCoinScore() const;
};

#endif // SURVIVING_SARNTAL_HUDRENDERER_H
