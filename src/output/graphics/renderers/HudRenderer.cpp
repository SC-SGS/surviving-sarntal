//
// Created by six on 9/23/24.
//

#include "HudRenderer.h"
#include "../GraphicsUtil.h"
#include "raymath.h"
#include "rlgl.h"

HudRenderer::HudRenderer(World &world, Camera2D &camera, GameConstants &gameConstants, ResourceManager &resourceManager)
    : world(world), camera(camera), gameConstants(gameConstants), resourceManager(resourceManager) {
    loadLandmarks();

    scoreTextureX = resourceManager.getTexture("scoreBgX");
    scoreTextureY = resourceManager.getTexture("scoreBgY");
}

void HudRenderer::loadLandmarks() { this->landmarks = ConfigManager::getInstance().getLandmarks(); }

void HudRenderer::renderHud(bool newDebugMode) {
    this->debugMode = newDebugMode;
    renderAltimeter();
    renderInventory();
    renderHealthBar();
    renderScore();
    renderCoinScore();

    DrawFPS(0, 0);
}

void HudRenderer::renderAltimeter() const {
    int stepSize = gameConstants.visualConstants.altimeterSteps *
                   static_cast<int>(graphics::UNIT_TO_PIXEL_RATIO); // Step size of the altimeter

    const int currentAltitude =
            static_cast<int>(world.getHiker().getPosition().y *
                             static_cast<int>(graphics::UNIT_TO_PIXEL_RATIO)); // Current altitude of the hiker
    const int topAltitude = currentAltitude - GetScreenHeight() / 2; // Top altitude of the screen
    const int bottomAltitude = currentAltitude + GetScreenHeight() / 2; // Bottom altitude of the screen

    for (int i = GraphicsUtil::floorToNearest(bottomAltitude, stepSize) + stepSize; i > topAltitude;
         i -= static_cast<int>(graphics::UNIT_TO_PIXEL_RATIO)) {
        const int drawY = GetScreenHeight() / 2 - (i - currentAltitude);
        const int drawAltitude =
                (i + gameConstants.visualConstants.cameraToHikerOffset) / static_cast<int>(
                    graphics::UNIT_TO_PIXEL_RATIO);

        renderAltimeterStep(drawY, drawAltitude, gameConstants.visualConstants.fontSizeAltimeter);
    }

    if (!this->debugMode) {
        for (const auto &landmark: landmarks) {
            const int altitude = (landmark.second - 1) * static_cast<int>(graphics::UNIT_TO_PIXEL_RATIO) -
                                 gameConstants.visualConstants.cameraToHikerOffset;
            const int drawY = (GetScreenHeight() / 2 - (altitude - currentAltitude));
            DrawLine(0, drawY, GetScreenWidth(), drawY, DARKGREEN);
            DrawText(landmark.first.c_str(), 4 * gameConstants.visualConstants.uiMargin,
                     drawY - gameConstants.visualConstants.fontSizeAltimeter,
                     gameConstants.visualConstants.fontSizeAltimeter, DARKGREEN);
        }
    }
}

void HudRenderer::renderAltimeterStep(const int drawY, const int drawAltitude, const int fontSize) const {
    rlSetLineWidth(1);
    if (drawAltitude % gameConstants.visualConstants.altimeterSteps == 0) {
        // right side
        DrawLine(GetScreenWidth(), drawY + fontSize / 2, GetScreenWidth() - gameConstants.visualConstants.uiMargin,
                 drawY + fontSize / 2, DARKGREEN);
        DrawText(std::to_string(drawAltitude).c_str(), GetScreenWidth() - gameConstants.visualConstants.uiMargin - 30,
                 drawY, fontSize, DARKGREEN);

        // left side
        DrawLine(0, drawY + fontSize / 2, gameConstants.visualConstants.uiMargin, drawY + fontSize / 2, DARKGREEN);
        DrawText(std::to_string(drawAltitude).c_str(), gameConstants.visualConstants.uiMargin + 10, drawY, fontSize,
                 DARKGREEN);
    } else if (drawAltitude % (gameConstants.visualConstants.altimeterSteps / 2) == 0) {
        // right side
        DrawLine(GetScreenWidth(), drawY + fontSize / 2, GetScreenWidth() - gameConstants.visualConstants.uiMargin,
                 drawY + fontSize / 2, DARKGREEN);
        // left side
        DrawLine(0, drawY + fontSize / 2, gameConstants.visualConstants.uiMargin, drawY + fontSize / 2, DARKGREEN);
    } else {
        // right side
        DrawLine(GetScreenWidth(), drawY + fontSize / 2, GetScreenWidth() - gameConstants.visualConstants.uiMargin / 2,
                 drawY + fontSize / 2, DARKGREEN);
        // left side
        DrawLine(0, drawY + fontSize / 2, gameConstants.visualConstants.uiMargin / 2, drawY + fontSize / 2, DARKGREEN);
    }
}

void HudRenderer::renderInventory() const {
    const auto &inventory = world.getInventory();
    const int slotSize = static_cast<int>(gameConstants.itemsConstants.inventorySlotSize);
    const int inventoryWidth = static_cast<int>(inventory.getNumberOfSlots()) * slotSize;
    const int startX = GetScreenWidth() - inventoryWidth - gameConstants.visualConstants.uiMargin;
    const int startY = GetScreenHeight() - slotSize - gameConstants.visualConstants.uiMargin;

    for (int i = 0; i < inventory.getNumberOfSlots(); i++) {
        // Draw Rectangle for each slot
        DrawRectangleLines(startX + i * slotSize, startY, slotSize, slotSize, WHITE);

        // Fill if slot is selected
        if (inventory.getSelectedSlot() == i) {
            DrawRectangle(startX + i * slotSize, startY, slotSize, slotSize, Fade(WHITE, 0.3f));
        }
        renderItemSlot(inventory, i, startX, startY);
    }
}

void HudRenderer::renderItemSlot(const Inventory &inventory,
                                 const int slotNumber,
                                 const int startX,
                                 const int startY) const {
    if (!inventory.slotIsEmpty(slotNumber)) {
        const auto textureName = inventory.getItem(slotNumber)->getRenderInformation().texture;
        const auto texture = resourceManager.getTexture(textureName);
        const floatType inventorySlotSize = gameConstants.itemsConstants.inventorySlotSize;
        DrawTexturePro(texture, {0, 0, static_cast<floatType>(texture.width), static_cast<floatType>(texture.height)},
                       {
                           static_cast<floatType>(startX) + static_cast<floatType>(slotNumber) * inventorySlotSize,
                           static_cast<floatType>(startY), inventorySlotSize, inventorySlotSize
                       },
                       {0, 0}, 0, WHITE);
        const size_t numberOfItems = inventory.getNumberOfItems(slotNumber);
        if (numberOfItems > 0) {
            DrawText(std::to_string(numberOfItems).c_str(),
                     static_cast<int>(startX) + slotNumber * static_cast<int>(inventorySlotSize) + 5,
                     static_cast<int>(startY) + 5, 20, WHITE);
        }
    }
}

void HudRenderer::renderHealthBar() const {
    const auto &hiker = world.getHiker();
    const auto health = static_cast<floatType>(hiker.getHealthPoints()) /
                        static_cast<floatType>(gameConstants.hikerConstants.hikerMaxHealth);
    const int screenWidth = GetScreenWidth();
    const int startX =
            screenWidth - gameConstants.visualConstants.healthBarWidth - gameConstants.visualConstants.uiMargin;
    const int startY = gameConstants.visualConstants.uiMargin;
    const auto healthBarFill =
            static_cast<int>(static_cast<floatType>(gameConstants.visualConstants.healthBarWidth) * health);

    // Draw the health bar
    DrawRectangle(startX, startY, gameConstants.visualConstants.healthBarWidth,
                  gameConstants.visualConstants.healthBarHeight, Fade(RED, 0.5f));
    DrawRectangleLines(startX, startY, gameConstants.visualConstants.healthBarWidth,
                       gameConstants.visualConstants.healthBarHeight, WHITE);
    DrawRectangle(startX, startY, healthBarFill, gameConstants.visualConstants.healthBarHeight, GREEN);
}

void HudRenderer::renderScore() const {
    int scoreX = static_cast<int>(this->world.getGameScore().x);
    int scoreY = static_cast<int>(this->world.getGameScore().y);

    std::string scoreXString = std::to_string(scoreX) + "m";
    std::string scoreYString = std::to_string(scoreY) + "m";
    const char *scoreXText = scoreXString.c_str();
    const char *scoreYText = scoreYString.c_str();

    if (!debugMode) {
        // Drawing textures using the helper function
        drawScoreTexture(scoreTextureX, 4, 2.0f);
        drawScoreTexture(scoreTextureY, 8, 1.3f);
    }

    // Drawing score texts using the helper function
    drawScoreText(scoreXText, 3);
    drawScoreText(scoreYText, 9);
}

void HudRenderer::drawScoreTexture(const Texture2D &texture,
                                   floatType widthOffset,
                                   floatType uiMarginMultiplier) const {
    DrawTexturePro(texture, {0, 0, static_cast<floatType>(texture.width), static_cast<floatType>(texture.height)},
                   {
                       static_cast<floatType>(GetScreenWidth() - gameConstants.visualConstants.uiMargin) -
                       static_cast<floatType>(gameConstants.visualConstants.fontSizeScore) * widthOffset,
                       static_cast<floatType>(gameConstants.visualConstants.uiMargin) * uiMarginMultiplier,
                       static_cast<floatType>(gameConstants.visualConstants.fontSizeScore * 4),
                       static_cast<floatType>(gameConstants.visualConstants.fontSizeScore * 3)
                   },
                   {0, 0}, 0, WHITE);
}

void HudRenderer::drawScoreText(const char *scoreText, int textXOffset) const {
    const auto centerX = GetScreenWidth() - MeasureText(scoreText, gameConstants.visualConstants.fontSizeScore) -
                         textXOffset * gameConstants.visualConstants.uiMargin;
    DrawText(scoreText, centerX, 3 * gameConstants.visualConstants.uiMargin,
             gameConstants.visualConstants.fontSizeScore, WHITE);
}

void HudRenderer::renderCoinScore() const {
    std::string scoreString = std::to_string(this->world.getCoinScore());
    const char *scoreText = scoreString.c_str();
    const auto centerX = GetScreenWidth() - MeasureText(scoreText, gameConstants.visualConstants.fontSizeScore) -
                         2 * gameConstants.visualConstants.uiMargin;
    DrawText(scoreText, centerX, gameConstants.visualConstants.uiMargin * 8,
             gameConstants.visualConstants.fontSizeScore, GOLD);
    const auto coinTexture = resourceManager.getTexture("coin");
    DrawTexturePro(resourceManager.getTexture("coin"),
                   {0, 0, static_cast<floatType>(coinTexture.width), static_cast<floatType>(coinTexture.height)},
                   {
                       static_cast<float>(GetScreenWidth() - gameConstants.visualConstants.uiMargin * 2),
                       static_cast<float>(gameConstants.visualConstants.uiMargin * 8),
                       static_cast<float>(gameConstants.visualConstants.fontSizeScore),
                       static_cast<float>(gameConstants.visualConstants.fontSizeScore)
                   },
                   {0, 0}, 0, WHITE);
}

void HudRenderer::reset() {
    /* Nothing to reset */
}
