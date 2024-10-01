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

// Main rendering function
void Renderer::draw() {
    handleFullScreenSwitch();
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

void Renderer::toggleFullscreen() {
    if (!IsWindowFullscreen()) {
        int monitor = GetCurrentMonitor();
        SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
        fsMode = TO_FS;
    } else {
        ToggleFullscreen();
        fsMode = TO_WINDOWED;
    }
}

void Renderer::handleFullScreenSwitch() {
    if (fsMode != NONE) {
        if (fsMode == TO_FS) {
            ToggleFullscreen();
            fsMode = UPDATE_VALUES;
        } else if (fsMode == TO_WINDOWED) {
            SetWindowSize(graphics::SCREEN_WIDTH_IN_PIXEL, graphics::SCREEN_HEIGHT_IN_PIXEL);
            fsMode = UPDATE_VALUES;
        } else if (fsMode == UPDATE_VALUES) {
            reset();
            fsMode = NONE;
        }
    }
    if (IsKeyPressed(KEY_F11)) {
        toggleFullscreen();
    }
}

void Renderer::setShake(const float intensity) { shakeIntensity = intensity; }

void Renderer::reset() {
    initCamera();
    mountainRenderer.reset();
    menuEngine.resetScreens();
    fsMode = NONE;
}
