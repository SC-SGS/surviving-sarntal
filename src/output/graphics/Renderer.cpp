#include "Renderer.h"
#include "../../spawner/PolygonGenerator.h"
#include "raylib.h"

#include "GraphicsUtil.h"
#include "renderers/EntityRenderer.h"
#include <cmath>

Renderer::Renderer(World &world,
                   ResourceManager &resourceManager,
                   Camera2D &camera,
                   GameConstants &gameConstants,
                   MountainRenderer &mountainRenderer,
                   EntityRenderer &entityRenderer,
                   HudRenderer &hudRenderer)
    : world(world),
      resourceManager(resourceManager),
      camera(camera),
      gameConstants(gameConstants),
      mountainRenderer(mountainRenderer),
      entityRenderer(entityRenderer),
      hudRenderer(hudRenderer) {

    initCamera();

    regenerateGradientTexture();
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

    // Initialize the camera
    camera.target = {centerX, centerY};
    camera.offset = screenCenter;
    camera.rotation = 0.0f;
    camera.zoom = zoom;
}

void Renderer::regenerateGradientTexture() {
    UnloadTexture(gradientTextureBackground); // TODO necessary?
    Image verticalGradient = GenImageGradientV(GetScreenWidth(), GetScreenHeight(), BLUE, WHITE);
    gradientTextureBackground = LoadTextureFromImage(verticalGradient);
    UnloadImage(verticalGradient);
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

    EndDrawing();
}

void Renderer::applyRumbleEffect() {
    if (shakeIntensity <= 0) {
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
    DrawTexture(gradientTextureBackground, 0, 0, WHITE);
    // Get the textures
    const Texture2D midgroundTex = resourceManager.getTexture("midground");
    const Texture2D foregroundTex = resourceManager.getTexture("foreground");

    scrollingMid -= 0.25f;
    scrollingFore -= 0.5f;

    const auto textureMidScale = gameConstants.visualConstants.textureMidScale;
    const auto textureForeScale = gameConstants.visualConstants.textureForeScale;

    const floatType midOffsetY =
        static_cast<floatType>(GetScreenHeight()) - static_cast<floatType>(midgroundTex.height) * textureMidScale;
    const floatType foreOffsetY =
        static_cast<floatType>(GetScreenHeight()) - static_cast<floatType>(foregroundTex.height) * textureForeScale;
    if (scrollingMid <= -static_cast<floatType>(midgroundTex.width) * textureMidScale) {
        scrollingMid = 0;
    }
    if (scrollingFore <= -static_cast<floatType>(foregroundTex.width) * textureForeScale) {
        scrollingFore = 0;
    }
    // Draw midground image repeatedly
    drawBackgroundTextureRepeatedly(midgroundTex, scrollingMid, textureMidScale, midOffsetY);
    // Draw foreground image repeatedly
    drawBackgroundTextureRepeatedly(foregroundTex, scrollingFore, textureForeScale, foreOffsetY);
}

void Renderer::drawBackgroundTextureRepeatedly(const Texture2D &texture2D,
                                               const floatType scrolling,
                                               const floatType scale,
                                               const floatType offsetY) const {

    DrawTextureEx(texture2D, {scrolling, offsetY}, 0.0f, scale, WHITE);
    DrawTextureEx(texture2D, {static_cast<floatType>(texture2D.width) * scale + scrolling, offsetY}, 0.0f, scale,
                  WHITE);
    DrawTextureEx(texture2D, {static_cast<floatType>(texture2D.width) * scale * 2 + scrolling, offsetY}, 0.0f, scale,
                  WHITE);
}

void Renderer::toggleDebugMode() { this->debugMode = !this->debugMode; }

void Renderer::setShake(const float intensity) { shakeIntensity = intensity; }

void Renderer::reset() {
    initCamera();
    mountainRenderer.reset();
    entityRenderer.reset();
}
