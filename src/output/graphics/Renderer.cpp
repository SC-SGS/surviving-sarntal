#include "Renderer.h"
#include "../../spawner/PolygonGenerator.h"
#include "raylib.h"

#include "GraphicsUtil.h"
#include <cmath>

Renderer::Renderer(World &world, ResourceManager &resourceManager, Camera2D &camera, MountainRenderer &mountainRenderer,
                   GameConstants gameConstants, PolygonRenderer &polygonRenderer)
    : world(world), resourceManager(resourceManager), camera(camera), mountainRenderer(mountainRenderer),
      gameConstants(gameConstants), polygonRenderer(polygonRenderer) {

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

    loadLandmarks();

    regenerateGradientTexture();
}

void Renderer::loadLandmarks() { this->landmarks = ConfigManager::getInstance().getLandmarks(); }

// Function to render an entity
void Renderer::renderEntity(const RenderedEntity &entity) const {
    renderEntity(entity, entity.getRenderInformation().rotationAngle);
}

void Renderer::renderEntity(const RenderedEntity &entity, const floatType rotation) const {
    auto info = entity.getRenderInformation();

    const Texture2D texture = resourceManager.getTexture(info.texture);
    auto directedWidth = static_cast<floatType>(texture.width * (info.width >= 0 ? 1 : -1));
    // part of the texture used
    const Rectangle sourceRec = {0.0f, 0.0f, directedWidth, static_cast<floatType>(texture.height)};
    renderEntity(entity, rotation, texture, sourceRec);
}

void Renderer::renderEntity(const RenderedEntity &entity, const floatType rotation, const Texture2D &texture,
                            const Rectangle sourceRec) const {
    auto info = entity.getRenderInformation();
    info.width *= graphics::UNIT_TO_PIXEL_RATIO;
    info.height *= graphics::UNIT_TO_PIXEL_RATIO;

    // Define the destination rectangle
    const Vector transformedPosition = GraphicsUtil::transformPosition(info.position, info.offset);
    const Rectangle destRec = {transformedPosition.x, transformedPosition.y, std::abs(info.width), info.height};

    // Define the origin for rotation.
    const Vector2 origin = {std::abs(info.width) / 2, info.height / 2};

    // Draw the texture if not in debug mode
    if (!this->debugMode) {
        DrawTexturePro(texture, sourceRec, destRec, origin, rotation, WHITE);
    } else {
        // Undo the width flip if negative
        const int transformedWidth = static_cast<int>(info.width) * (info.width >= 0 ? 1 : -1);
        // Draw Rectangle for collision box, center with width and height
        DrawRectangleLines(static_cast<int>(destRec.x - destRec.width / 2),
                           static_cast<int>(destRec.y - destRec.height / 2), transformedWidth,
                           static_cast<int>(info.height), RED);
    }
}

void Renderer::animateEntity(const RenderedEntity &entity) {
    auto info = entity.getRenderInformation();
    const auto currentTime = static_cast<floatType>(GetTime());

    AnimationInformation &animation = getAnimationInformation(entity.getId(), info.animation);

    // Check if we need to advance to the next frame
    if (currentTime >= animation.lastTime + animation.frameTime) {
        // Advance to the next frame
        animation.currentFrame = (animation.currentFrame + 1) % animation.frames;

        // Reset current time
        animation.lastTime = currentTime;
    }
    animations.insert_or_assign(entity.getId(), animation);

    renderAnimation(entity);
}

void Renderer::renderAnimation(const RenderedEntity &entity) {
    // Get render and animation information
    auto info = entity.getRenderInformation();
    auto animation = getAnimationInformation(entity.getId(), info.animation);

    // Get the texture
    const Texture2D texture = resourceManager.getTexture(info.texture);

    // Calculate frame width based on the texture and animation frames
    const floatType frameWidth = static_cast<floatType>(texture.width) / static_cast<floatType>(info.animation.frames);
    const floatType width = frameWidth * static_cast<floatType>(info.width >= 0 ? 1 : -1);

    // Calculate current frame's X position in the texture
    const floatType currentFrameX = frameWidth * static_cast<floatType>(animation.currentFrame);

    // Flip horizontally if width is negative
    floatType directedFrameX = info.width >= 0 ? currentFrameX : currentFrameX + frameWidth;
    // Create source rectangle for the current frame
    Rectangle sourceRec = {directedFrameX, 0.0f, width, static_cast<floatType>(texture.height)};

    // Render the entity with the updated frame
    this->renderEntity(entity, info.rotationAngle, texture, sourceRec);
}

void Renderer::renderHiker(const Hiker &hiker) {
    HikerMovement::MovementState state = world.getHiker().getHikerMovement().getState();

    switch (state) {
    case HikerMovement::MovementState::IN_AIR:
        renderEntity(hiker);
        break;
    case HikerMovement::MovementState::MOVING:
        renderWalkingHiker(hiker);
        break;
    case HikerMovement::MovementState::CROUCHED:
        renderEntity(hiker);
        break;
    }
}

void Renderer::renderWalkingHiker(const Hiker &hiker) {
    HikerMovement::Direction direction = world.getHiker().getHikerMovement().getDirection();

    if (direction == HikerMovement::Direction::NEUTRAL) {
        // Get the texture
        const Texture2D texture = resourceManager.getTexture(hiker.getRenderInformation().texture);
        const floatType width = static_cast<floatType>(texture.width) /
                                static_cast<floatType>(hiker.getRenderInformation().animation.frames);
        Rectangle sourceRec = {0.0f, 0.0f, width, static_cast<floatType>(texture.height)};
        renderEntity(hiker, hiker.getRenderInformation().rotationAngle, texture, sourceRec);
    } else {
        animateEntity(hiker);
    }
}

void Renderer::renderRock(const Rock &rock) const { polygonRenderer.renderTexturedPolygon(rock); }

void Renderer::debugRenderRock(const Rock &rock) const {
    polygonRenderer.renderPolygonOutline(rock);
    /*
    const auto transformedPosition = GraphicsUtil::transformPosition(entity.getRenderInformation().position);

    // Draw Circle for collision box
    DrawCircleLines(static_cast<int>(transformedPosition.x), static_cast<int>(transformedPosition.y),
                    (entity.getRenderInformation().width * graphics::UNIT_TO_PIXEL_RATIO) / 2, RED);

    // Calculate the end points of the line to detect rotation
    const floatType rotation = entity.getRenderInformation().angularOffset;
    const floatType radius = entity.getRenderInformation().width * graphics::UNIT_TO_PIXEL_RATIO / 2;
    const floatType rotation = entity.getRenderInformation().rotationAngle;
    const floatType radius = entity.getRenderInformation().width / 2;
    const int endX = static_cast<int>(transformedPosition.x + radius * std::cos(rotation));
    const int endY = static_cast<int>(transformedPosition.y + radius * std::sin(rotation));

    // Draw the line
    DrawLine(static_cast<int>(transformedPosition.x), static_cast<int>(transformedPosition.y), endX, endY, RED);
    */
}

void Renderer::renderEntities() {
    if (!this->debugMode)
        renderNormalEntities();
    else {
        debugRenderEntities();
    }
}

void Renderer::renderNormalEntities() {
    auto &hiker = world.getHiker();
    auto &rocks = world.getRocks();
    auto &monster = world.getMonster();

    // Render destroyed rocks, e.g. explosions
    for (auto &destroyedRock : getDestroyedRocks()) {
        animateEntity(destroyedRock);
    }

    // Render hiker
    renderHiker(hiker);

    // Render rocks
    for (auto &rock : rocks) {
        renderRock(rock);
    }

    // Render monster
    animateEntity(monster);

    // Render Items
    for (const auto &item : world.getItems()) {
        renderEntity(*item);
    }
}

void Renderer::debugRenderEntities() {
    const auto &hiker = world.getHiker();
    const auto &rocks = world.getRocks();
    const auto &monster = world.getMonster();
    const auto &terrain = world.getTerrain();
    // Render hiker
    renderHiker(hiker);
    // Render rocks
    for (auto &rock : rocks) {
        debugRenderRock(rock);
    }
    // Render monster
    DrawLine(static_cast<int>(monster.getRenderInformation().position.x * graphics::UNIT_TO_PIXEL_RATIO),
             static_cast<int>(camera.target.y - (0.5 * GetScreenHeight())),
             static_cast<int>(monster.getRenderInformation().position.x * graphics::UNIT_TO_PIXEL_RATIO),
             GetScreenHeight(), RED);
    // Render Items
    for (const auto &item : world.getItems()) {
        const auto transformedPosition = GraphicsUtil::transformPosition(item->getRenderInformation().position);
        renderEntity(*item);
        constexpr auto FONT_SIZE = 20; // TODO magic number alert
        const auto itemType = item->getRenderInformation().texture.c_str();
        const auto centerX = static_cast<int>(transformedPosition.x) - MeasureText(itemType, FONT_SIZE) / 2;
        DrawText(itemType, centerX, static_cast<int>(transformedPosition.y), FONT_SIZE, GREEN);
        DrawCircleLines(static_cast<int>(transformedPosition.x), static_cast<int>(transformedPosition.y),
                        gameConstants.itemsConstants.collectionRadius * graphics::UNIT_TO_PIXEL_RATIO, BLUE);
    }
}

void Renderer::renderHUD() const {
    renderAltimeter();
    renderInventory();
    renderHealthBar();
    renderScore();
    renderCoinScore();

    DrawFPS(0, 0);
}

void Renderer::renderItemSlot(const Inventory &inventory, const int slotNumber, const int startX,
                              const int startY) const {
    if (!inventory.slotIsEmpty(slotNumber)) {
        const auto textureName = inventory.getItem(slotNumber)->getRenderInformation().texture;
        const auto texture = resourceManager.getTexture(textureName);
        const floatType inventorySlotSize = gameConstants.itemsConstants.inventorySlotSize;
        DrawTexturePro(texture, {0, 0, static_cast<floatType>(texture.width), static_cast<floatType>(texture.height)},
                       {static_cast<floatType>(startX) + static_cast<floatType>(slotNumber) * inventorySlotSize,
                        static_cast<floatType>(startY), inventorySlotSize, inventorySlotSize},
                       {0, 0}, 0, WHITE);
        const size_t numberOfItems = inventory.getNumberOfItems(slotNumber);
        if (numberOfItems > 0) {
            DrawText(std::to_string(numberOfItems).c_str(),
                     static_cast<int>(startX) + slotNumber * static_cast<int>(inventorySlotSize) + 5,
                     static_cast<int>(startY) + 5, 20, WHITE);
        }
    }
}

void Renderer::renderInventory() const {
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

void Renderer::renderHealthBar() const {
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

void Renderer::renderCoinScore() const {
    std::string scoreString = std::to_string(this->world.getCoinScore());
    const char *scoreText = scoreString.c_str();
    const auto centerX = GetScreenWidth() - MeasureText(scoreText, gameConstants.visualConstants.fontSizeScore) -
                         2 * gameConstants.visualConstants.uiMargin;
    DrawText(scoreText, centerX, gameConstants.visualConstants.uiMargin * 6,
             gameConstants.visualConstants.fontSizeScore, GOLD);
    const auto coinTexture = resourceManager.getTexture("coin");
    DrawTexturePro(resourceManager.getTexture("coin"),
                   {0, 0, static_cast<floatType>(coinTexture.width), static_cast<floatType>(coinTexture.height)},
                   {static_cast<float>(GetScreenWidth() - gameConstants.visualConstants.uiMargin * 2),
                    static_cast<float>(gameConstants.visualConstants.uiMargin * 6),
                    static_cast<float>(gameConstants.visualConstants.fontSizeScore),
                    static_cast<float>(gameConstants.visualConstants.fontSizeScore)},
                   {0, 0}, 0, WHITE);
}

void Renderer::renderScore() const {
    std::string scoreString = std::to_string(this->world.getGameScore()) + "m";
    const char *scoreText = scoreString.c_str();
    const auto centerX = GetScreenWidth() - MeasureText(scoreText, gameConstants.visualConstants.fontSizeScore) -
                         2 * gameConstants.visualConstants.uiMargin;
    DrawText(scoreText, centerX, gameConstants.visualConstants.uiMargin * 3,
             gameConstants.visualConstants.fontSizeScore, WHITE);
}

void Renderer::renderAltimeter() const {
    int stepSize = gameConstants.visualConstants.altimeterSteps *
                   static_cast<int>(graphics::UNIT_TO_PIXEL_RATIO); // Step size of the altimeter

    const int currentAltitude =
        static_cast<int>(world.getHiker().getPosition().y *
                         static_cast<int>(graphics::UNIT_TO_PIXEL_RATIO)); // Current altitude of the hiker
    const int topAltitude = currentAltitude - GetScreenHeight() / 2;       // Top altitude of the screen
    const int bottomAltitude = currentAltitude + GetScreenHeight() / 2;    // Bottom altitude of the screen

    for (int i = GraphicsUtil::floorToNearest(bottomAltitude, stepSize) + stepSize; i > topAltitude;
         i -= static_cast<int>(graphics::UNIT_TO_PIXEL_RATIO)) {
        const int drawY = GetScreenHeight() / 2 - (i - currentAltitude);
        const int drawAltitude =
            (i + gameConstants.visualConstants.cameraToHikerOffset) / static_cast<int>(graphics::UNIT_TO_PIXEL_RATIO);

        renderAltimeterStep(drawY, drawAltitude, gameConstants.visualConstants.fontSizeAltimeter);
    }

    if (!this->debugMode) {
        for (const auto &landmark : landmarks) {
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

void Renderer::renderAltimeterStep(const int drawY, const int drawAltitude, const int fontSize) const {
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

AnimationInformation &Renderer::getAnimationInformation(int entityId, AnimationInformation &defaultAnimation) {
    if (animations.find(entityId) == animations.end()) {
        animations[entityId] = defaultAnimation;
    }
    return animations[entityId];
}

void Renderer::addExplosion(const Rock &rock) const {
    // TODO the explosion needs to fit the bounding box
    Rock destroyedRock(rock.getPosition(), rock.getBodySpaceVertices(), rock.getTextureCoordinates(), 0.0f, 0.0f,
                       DynamicProperties());
    destroyedRock.setAnimationInformation({25, 0, 0.1, 0});
    this->destroyedRocks->push_back(destroyedRock);
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

    renderEntities();

    mountainRenderer.renderMountain(world.getTerrain(), WHITE, SKYBLUE, debugMode);

    EndMode2D();

    renderHUD();

    EndDrawing();
}

void Renderer::setShake(const float intensity) { shakeIntensity = intensity; }

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

void Renderer::drawBackgroundTextureRepeatedly(const Texture2D &texture2D, const floatType scrolling,
                                               const floatType scale, const floatType offsetY) const {

    DrawTextureEx(texture2D, {scrolling, offsetY}, 0.0f, scale, WHITE);
    DrawTextureEx(texture2D, {static_cast<floatType>(texture2D.width) * scale + scrolling, offsetY}, 0.0f, scale,
                  WHITE);
    DrawTextureEx(texture2D, {static_cast<floatType>(texture2D.width) * scale * 2 + scrolling, offsetY}, 0.0f, scale,
                  WHITE);
}

void Renderer::regenerateGradientTexture() {
    UnloadTexture(gradientTextureBackground); // TODO necessary?
    Image verticalGradient = GenImageGradientV(GetScreenWidth(), GetScreenHeight(), BLUE, WHITE);
    gradientTextureBackground = LoadTextureFromImage(verticalGradient);
    UnloadImage(verticalGradient);
}

void Renderer::toggleDebugMode() { this->debugMode = !this->debugMode; }

std::list<Rock> &Renderer::getDestroyedRocks() const {
    destroyedRocks->remove_if([this](Rock &rock) {
        auto animation = animations.find(rock.getId());
        if (animation != animations.end()) {
            return animation->second.currentFrame >= animation->second.frames - 1;
        } else {
            return false;
        }
    });
    return *destroyedRocks;
}
