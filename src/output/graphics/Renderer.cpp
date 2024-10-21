#include "Renderer.h"
#include "../../spawner/PolygonGenerator.h"
#include "raylib.h"

#include "../../physics/PhysicsEngine.hpp"
#include "GraphicsUtil.h"
#include "renderers/EntityRenderer.h"
#include <cmath>

Renderer::Renderer(World &world,
                   ResourceManager &resourceManager,
                   Camera2D &camera,
                   GameConstants &gameConstants,
                   MenuEngine &menuEngine,
                   MountainRenderer &mountainRenderer,
                   EntityRenderer &entityRenderer,
                   HudRenderer &hudRenderer)
    : world(world),
      resourceManager(resourceManager),
      camera(camera),
      gameConstants(gameConstants),
      menuEngine(menuEngine),
      mountainRenderer(mountainRenderer),
      entityRenderer(entityRenderer),
      hudRenderer(hudRenderer) {

    initCamera();
}

void Renderer::initCamera() {
    const floatType leftBorder = world.getMinX() * graphics::UNIT_TO_PIXEL_RATIO;
    const floatType rightBorder = world.getMaxX() * graphics::UNIT_TO_PIXEL_RATIO;

    // Calculate the world width
    const floatType worldWidth = rightBorder - leftBorder;

    // Calculate the zoom level so that the world width fits the screen width
    const auto screenWidth = static_cast<floatType>(GetScreenWidth());
    const floatType zoom = screenWidth / worldWidth;

    // Calculate the visible width and height
    const floatType visibleWidth = rightBorder - leftBorder;
    const floatType ratio = static_cast<floatType>(GetScreenHeight()) / static_cast<floatType>(GetScreenWidth());
    const floatType visibleHeight = visibleWidth * ratio;

    // Calculate the center of the camera view based on the borders
    const floatType centerX = (leftBorder + rightBorder) / 2.0f;
    const floatType centerY = visibleHeight / 2.0f;
    screenCenter = {static_cast<float>(GetScreenWidth()) / 2, static_cast<float>(GetScreenHeight()) / 2};

    // Initialize the camera
    camera.target = {centerX, centerY};
    camera.offset = screenCenter;
    camera.rotation = 0.0f;
    camera.zoom = zoom;
}

void Renderer::renderPhysicsTimeStepSize() const {
    if (!this->gameConstants.visualConstants.renderPhysicsStepSize)
        return;
    // Convert the double to a string
    const std::string valueText = std::to_string(PhysicsEngine::exponentialMovingAvg);

    // Determine the position (bottom-left corner)
    int textX = 20;                     // Small padding from the left edge
    int textY = GetScreenHeight() - 40; // Padding from the bottom edge

    // Draw the text using Raylib's DrawText function (convert std::string to C-string using c_str())
    DrawText(valueText.c_str(), textX, textY, 30, BLACK); // Font size = 20, color = white
}

// Main rendering function
void Renderer::draw() {
    applyRumbleEffect();

    BeginDrawing();
    ClearBackground(BLACK);

    // Render background if not in debug mode
    renderBackground();

    // Adjust y-position of camera
    camera.target.y =
        GraphicsUtil::transformYCoordinate(world.getHiker().getRenderInformation().position.y +
                                           static_cast<floatType>(gameConstants.visualConstants.cameraToHikerOffset));
    camera.target.x = (world.getMaxX() + world.getMinX()) * graphics::UNIT_TO_PIXEL_RATIO / 2.0f;

    BeginMode2D(camera);

    entityRenderer.renderEntities(debugMode);

    mountainRenderer.renderMountain(world.getTerrain(), debugMode);

    EndMode2D();

    hudRenderer.renderHud(debugMode);

    renderPhysicsTimeStepSize();

    EndDrawing();
}

void Renderer::applyRumbleEffect() {
    if (shakeIntensity <= 0 || !this->gameConstants.visualConstants.shakeEnabled) {
        return;
    }

    if (shakeIntensity > 0.01f) {
        shakeIntensity *= gameConstants.visualConstants.rumbleDampening;
        const int shakeIntensityInt = static_cast<int>(shakeIntensity);
        camera.offset.x = screenCenter.x + static_cast<float>(GetRandomValue(-shakeIntensityInt, shakeIntensityInt));
        camera.offset.y = screenCenter.y + static_cast<float>(GetRandomValue(-shakeIntensityInt, shakeIntensityInt));
    } else {
        shakeIntensity = 0.0f;
        camera.offset = screenCenter;
    }
}

void Renderer::renderBackground() {
    if (this->debugMode) {
        return;
    }

    const Texture2D &backgroundTexture = resourceManager.getTexture("background");
    const float drawnWidth = static_cast<float>(backgroundTexture.width) /
                             static_cast<float>(backgroundTexture.height) * static_cast<float>(GetScreenHeight());
    float offset = world.getMonster().getPosition().x * graphics::UNIT_TO_PIXEL_RATIO *
                   gameConstants.visualConstants.backgroundDistanceScale;

    while (offset > static_cast<float>(backgroundTexture.width)) {
        offset -= static_cast<float>(backgroundTexture.width);
    }
    drawBackgroundTextureRepeatedly(backgroundTexture, drawnWidth, offset);
}

void Renderer::drawBackgroundTextureRepeatedly(const Texture2D &texture,
                                               const floatType drawnWidth,
                                               const floatType offset) const {

    float coveredDistance = -offset;
    while (coveredDistance < static_cast<float>(GetScreenWidth())) {
        Rectangle const destRect = Rectangle{coveredDistance, 0, drawnWidth, static_cast<float>(GetScreenHeight())};
        DrawTexturePro(texture, Rectangle{0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height)},
                       destRect, Vector2{0, 0}, 0.0f, WHITE);

        coveredDistance += drawnWidth;
    }
}

void Renderer::toggleDebugMode() { this->debugMode = !this->debugMode; }

void Renderer::addShake(const float intensity) { shakeIntensity += intensity; }

void Renderer::reset() {
    mountainRenderer.reset();
    menuEngine.resetScreens();
    initCamera();
}
