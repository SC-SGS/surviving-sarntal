#include "Renderer.h"
#include "raylib.h"

#include <cmath>
#include <mutex>

Renderer::Renderer(World &world, ResourceManager &resourceManager) : world(world), resourceManager(resourceManager) {

    const floatType leftBorder = world.getMinX();
    const floatType rightBorder = world.getMaxX();

    // Calculate the world width
    const floatType worldWidth = rightBorder - leftBorder;

    // Calculate the zoom level so that the world width fits the screen width
    const auto screenWidth = static_cast<floatType>(GetScreenWidth());
    const floatType zoom = screenWidth / worldWidth;

    // Calculate the visible width and height
    const floatType visibleWidth = rightBorder - leftBorder;
    const floatType visibleHeight =
        visibleWidth * (static_cast<floatType>(GetScreenHeight()) / static_cast<floatType>(GetScreenWidth()));

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
void Renderer::renderEntity(RenderedEntity &entity) const {
    renderEntity(entity, entity.getRenderInformation().angularOffset);
}

void Renderer::renderEntity(RenderedEntity &entity, const floatType rotation) const {
    const Texture2D texture = resourceManager.getTexture(entity.getRenderInformation().texture);
    const Rectangle sourceRec = {0.0f, 0.0f, static_cast<floatType>(texture.width),
                                 static_cast<floatType>(texture.height)}; // part of the texture used
    renderEntity(entity, rotation, texture, sourceRec);
}

void Renderer::renderEntity(RenderedEntity &entity, const floatType rotation, const Texture2D &texture,
                            const Rectangle sourceRec) const {
    const auto info = entity.getRenderInformation();
    // Define the destination rectangle
    const Vector transformedPosition = this->transformPosition(info.position, info.offset);
    const Rectangle destRec = {transformedPosition.x, transformedPosition.y, info.width, info.height};

    // Define the origin for rotation.
    const Vector2 origin = {info.width / 2, info.height / 2};

    // Draw the texture if not in debug mode
    if (!this->debugMode) {
        DrawTexturePro(texture, sourceRec, destRec, origin, rotation, WHITE);
    } else {
        DrawTexturePro(texture, sourceRec, destRec, origin, rotation, WHITE);
        // Draw Rectangle for collision box, center with width and height
        DrawRectangleLines(static_cast<int>(destRec.x - destRec.width / 2),
                           static_cast<int>(destRec.y - destRec.height / 2), static_cast<int>(info.width),
                           static_cast<int>(info.height), RED);
    }
}

void Renderer::animateEntity(RenderedEntity &entity) const {
    auto info = entity.getRenderInformation();
    const auto currentTime = static_cast<floatType>(GetTime());
    AnimationInformation &animation = info.animation;

    // Check if we need to advance to the next frame
    if (currentTime >= animation.lastTime + animation.frameTime) {
        // Advance to the next frame
        animation.currentFrame = (animation.currentFrame + 1) % animation.frames;

        // Reset current time
        animation.lastTime = currentTime;
    }
    entity.setAnimationInformation(animation);

    renderAnimation(entity);
}

void Renderer::renderAnimation(RenderedEntity &entity) const {
    auto info = entity.getRenderInformation();
    // Get the texture and calculate frame width
    const Texture2D texture = resourceManager.getTexture(info.texture);
    const floatType width = static_cast<floatType>(texture.width) / static_cast<floatType>(info.animation.frames);
    const floatType currentFrameWidth = width * static_cast<floatType>(info.animation.currentFrame);

    // Create source rectangle for the current frame
    Rectangle sourceRec = {currentFrameWidth, 0.0f, width, static_cast<floatType>(texture.height)};

    // Update render information for the entity
    info.width = width;
    info.height = static_cast<floatType>(texture.height);

    // Render the entity with the updated frame
    this->renderEntity(entity, info.angularOffset, texture, sourceRec);
}

void Renderer::renderHiker(RenderedEntity &hiker) const {
    if (hiker.getRenderInformation().texture == "walk") {
        animateEntity(hiker);
    } else {
        renderEntity(hiker);
    }
}

void Renderer::renderRock(RenderedEntity &entity) const {
    renderEntity(entity, entity.getRenderInformation().angularOffset);
}
void Renderer::debugRenderRock(RenderedEntity &entity) const {
    const auto transformedPosition = transformPosition(entity.getRenderInformation().position);

    // Draw Circle for collision box
    DrawCircleLines(static_cast<int>(transformedPosition.x), static_cast<int>(transformedPosition.y),
                    entity.getRenderInformation().width / 2, RED);

    // Calculate the end points of the line to detect rotation
    const floatType rotation = entity.getRenderInformation().angularOffset;
    const floatType radius = entity.getRenderInformation().width / 2;
    const int endX = static_cast<int>(transformedPosition.x + radius * std::cos(rotation));
    const int endY = static_cast<int>(transformedPosition.y + radius * std::sin(rotation));

    // Draw the line
    DrawLine(static_cast<int>(transformedPosition.x), static_cast<int>(transformedPosition.y), endX, endY, RED);
}

void Renderer::renderMountain(const Mountain &mountain, Color topColor, Color bottomColor) const {
    // Retrieve the relevant section of the mountain to be displayed
    const IndexIntervalNew indexInterval = mountain.getIndexIntervalOfEntireMountain();
    // Get lower border of screen
    const floatType lowerBorder =
        camera.target.y + (static_cast<floatType>(GetScreenHeight()) / (2.0f * camera.zoom)) + 100.0f;
    // Load the texture
    const Texture2D texture = resourceManager.getTexture("mountain");
    const Rectangle sourceRec = {0.0f, 0.0f, static_cast<floatType>(texture.width),
                                 static_cast<floatType>(texture.height)};
    // Define origin and rotation
    floatType rotation = 0.0f; // TODO what is this?
    // Draw the mountain
    const int vertexOffset = 1; // TODO why these magic numberes?
    for (size_t i = indexInterval.startIndex; i < indexInterval.endIndex - vertexOffset; i += vertexOffset) {
        const Vector pos1 = transformPosition(Vector2(mountain.getVertex(i)));
        const Vector pos2 = transformPosition(Vector2(mountain.getVertex(i + vertexOffset)));
        // Render the mountain depending on debug mode
        if (!this->debugMode) {
            // Define destination rectangle (where to draw the texture, size of the texture in the destination)
            const Rectangle destRec1 = {pos1.x, pos1.y, pos2.x - pos1.x,
                                        lowerBorder - pos1.y}; // todo make so it is not so pixelated
            DrawTexturePro(texture, sourceRec, destRec1, {0.0f, 0.0f}, rotation, WHITE);
        } else {
            // Draw the line
            DrawLine(static_cast<int>(pos1.x), static_cast<int>(pos1.y), static_cast<int>(pos2.x),
                     static_cast<int>(pos2.y), RED);
        }
    }
}

void Renderer::renderEntities() const {
    if (!this->debugMode)
        renderNormalEntities();
    else {
        debugRenderEntities();
    }
}

void Renderer::renderNormalEntities() const {
    auto &hiker = world.getHiker();
    auto &rocks = world.getRocks();
    auto &destroyedRocks = world.getDestroyedRocks();
    auto &monster = world.getMonster();
    const auto &mountain = world.getMountain();

    // Render destroyed rocks, e.g. explosions
    for (auto &destroyedRock : destroyedRocks) {
        animateEntity(destroyedRock);
    }

    // Render hiker
    renderHiker(hiker);

    // Render rocks
    for (auto &rock : rocks) {
        renderEntity(rock);
    }

    // Render monster
    animateEntity(monster);

    // Render Items
    for (const auto &item : world.getItems()) {
        renderEntity(*item);
    }

    // Render mountain
    renderMountain(mountain, SKYBLUE, BLUE);
}

void Renderer::debugRenderEntities() const {
    auto &hiker = world.getHiker();
    auto &rocks = world.getRocks();
    auto &monster = world.getMonster();
    const auto &mountain = world.getMountain();
    // Render hiker
    renderHiker(hiker);
    // Render rocks
    for (auto &rock : rocks) {
        debugRenderRock(rock);
    }
    // Render monster
    DrawLine(static_cast<int>(monster.getRenderInformation().position.x),
             static_cast<int>(camera.target.y - (0.5 * GetScreenHeight())),
             static_cast<int>(monster.getRenderInformation().position.x), GetScreenHeight(), RED);
    // Render Items
    for (const auto &item : world.getItems()) {
        const auto transformedPosition = transformPosition(item->getRenderInformation().position);
        renderEntity(*item);
        constexpr auto FONT_SIZE = 20; // TODO magic number alert
        const auto itemType = item->getRenderInformation().texture.c_str();
        const auto centerX = static_cast<int>(transformedPosition.x) - MeasureText(itemType, FONT_SIZE) / 2;
        DrawText(itemType, centerX, static_cast<int>(transformedPosition.y), FONT_SIZE, GREEN);
        DrawCircleLines(static_cast<int>(transformedPosition.x), static_cast<int>(transformedPosition.y),
                        HIKER_ITEM_COLLECTION_RANGE, BLUE);
    }
    // Render mountain
    renderMountain(mountain, SKYBLUE, BLUE);
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
        DrawTexturePro(texture, {0, 0, static_cast<floatType>(texture.width), static_cast<floatType>(texture.height)},
                       {static_cast<floatType>(startX) + static_cast<floatType>(slotNumber) * INVENTORY_SLOT_SIZE,
                        static_cast<floatType>(startY), INVENTORY_SLOT_SIZE, INVENTORY_SLOT_SIZE},
                       {0, 0}, 0, WHITE);
        const size_t numberOfItems = inventory.getNumberOfItems(slotNumber);
        if (numberOfItems > 0) {
            DrawText(std::to_string(numberOfItems).c_str(),
                     static_cast<int>(startX) + slotNumber * static_cast<int>(INVENTORY_SLOT_SIZE) + 5,
                     static_cast<int>(startY) + 5, 20, WHITE);
        }
    }
}

void Renderer::renderInventory() const {
    const auto &inventory = world.getInventory();

    const int inventoryWidth = static_cast<int>(inventory.getNumberOfSlots()) * static_cast<int>(INVENTORY_SLOT_SIZE);
    const int startX = GetScreenWidth() - inventoryWidth - UI_MARGIN;
    const int startY = GetScreenHeight() - static_cast<int>(INVENTORY_SLOT_SIZE) - UI_MARGIN;

    for (int i = 0; i < inventory.getNumberOfSlots(); i++) {
        // Draw Rectangle for each slot
        DrawRectangleLines(startX + i * static_cast<int>(INVENTORY_SLOT_SIZE), startY, INVENTORY_SLOT_SIZE,
                           INVENTORY_SLOT_SIZE, WHITE);

        // Fill if slot is selected
        if (inventory.getSelectedSlot() == i) {
            DrawRectangle(startX + i * static_cast<int>(INVENTORY_SLOT_SIZE), startY, INVENTORY_SLOT_SIZE,
                          INVENTORY_SLOT_SIZE, Fade(WHITE, 0.3f));
        }
        renderItemSlot(inventory, i, startX, startY);
    }
}

void Renderer::renderHealthBar() const {
    const auto &hiker = world.getHiker();
    const floatType health = static_cast<floatType>(hiker.getHealthPoints()) / HIKER_MAX_HEALTH;
    const int screenWidth = GetScreenWidth();
    const int startX = screenWidth - HEALTH_BAR_WIDTH - UI_MARGIN;
    const int startY = UI_MARGIN;

    // Draw the health bar
    DrawRectangle(startX, startY, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT, Fade(RED, 0.5f));
    DrawRectangleLines(startX, startY, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT, WHITE);
    DrawRectangle(startX, startY, static_cast<int>(HEALTH_BAR_WIDTH * health), HEALTH_BAR_HEIGHT, GREEN);
}

void Renderer::renderCoinScore() const {
    const char *scoreText = (std::to_string(this->world.getCoinScore())).c_str();
    const auto centerX = GetScreenWidth() - MeasureText(scoreText, FONT_SIZE_SCORE) - 2 * UI_MARGIN;
    DrawText(scoreText, centerX, UI_MARGIN * 6, FONT_SIZE_SCORE, GOLD);
    const auto coinTexture = resourceManager.getTexture("coin");
    DrawTexturePro(resourceManager.getTexture("coin"),
                   {0, 0, static_cast<floatType>(coinTexture.width), static_cast<floatType>(coinTexture.height)},
                   {static_cast<float>(GetScreenWidth() - UI_MARGIN * 2), UI_MARGIN * 6,
                    static_cast<float>(FONT_SIZE_SCORE), static_cast<float>(FONT_SIZE_SCORE)},
                   {0, 0}, 0, WHITE);
}

void Renderer::renderScore() const {
    const char *scoreText = (std::to_string(this->world.getGameScore() / POSITION_TO_SCORE_RATIO) + "m").c_str();
    const auto centerX = GetScreenWidth() - MeasureText(scoreText, FONT_SIZE_SCORE) - 2 * UI_MARGIN;
    DrawText(scoreText, centerX, UI_MARGIN * 3, FONT_SIZE_SCORE, WHITE);
}

void Renderer::renderAltimeter() const {
    constexpr int STEP_SIZE = ALTIMETER_STEPS * POSITION_TO_SCORE_RATIO; // Step size of the altimeter
    // TODO remove - when x axis inverted
    const int currentAltitude = static_cast<int>(world.getHiker().getPosition().y); // Current altitude of the hiker
    const int topAltitude = currentAltitude - GetScreenHeight() / 2;                // Top altitude of the screen
    const int bottomAltitude = currentAltitude + GetScreenHeight() / 2;             // Bottom altitude of the screen

    for (int i = floorToNearest(bottomAltitude, STEP_SIZE); i > topAltitude; i -= POSITION_TO_SCORE_RATIO) {
        const int drawY = GetScreenHeight() / 2 - (i - currentAltitude);
        const int drawAltitude = i / POSITION_TO_SCORE_RATIO;

        renderAltimeterStep(drawY, drawAltitude, FONT_SIZE_ALTIMETER);
    }

    for (const auto &landmark : landmarks) {
        const int altitude = landmark.second * POSITION_TO_SCORE_RATIO;
        const int drawY = (GetScreenHeight() / 2 - (altitude - currentAltitude));
        DrawLine(0, drawY, GetScreenWidth(), drawY, DARKGREEN);
        DrawText(landmark.first.c_str(), 2 * UI_MARGIN, drawY - FONT_SIZE_ALTIMETER, FONT_SIZE_ALTIMETER, DARKGREEN);
    }
}

void Renderer::renderAltimeterStep(const int drawY, const int drawAltitude, const int fontSize) const {
    if (drawAltitude % ALTIMETER_STEPS == 0) {
        // right side
        DrawLine(GetScreenWidth(), drawY + fontSize / 2, GetScreenWidth() - UI_MARGIN, drawY + fontSize / 2, DARKGREEN);
        DrawText(std::to_string(drawAltitude).c_str(), GetScreenWidth() - UI_MARGIN - 30, drawY, fontSize, DARKGREEN);

        // left side
        DrawLine(0, drawY + fontSize / 2, UI_MARGIN, drawY + fontSize / 2, DARKGREEN);
        DrawText(std::to_string(drawAltitude).c_str(), UI_MARGIN + 10, drawY, fontSize, DARKGREEN);
    } else if (drawAltitude % (ALTIMETER_STEPS / 2) == 0) {
        // right side
        DrawLine(GetScreenWidth(), drawY + fontSize / 2, GetScreenWidth() - UI_MARGIN, drawY + fontSize / 2, DARKGREEN);
        // left side
        DrawLine(0, drawY + fontSize / 2, UI_MARGIN, drawY + fontSize / 2, DARKGREEN);
    } else {
        // right side
        DrawLine(GetScreenWidth(), drawY + fontSize / 2, GetScreenWidth() - UI_MARGIN / 2, drawY + fontSize / 2,
                 DARKGREEN);
        // left side
        DrawLine(0, drawY + fontSize / 2, UI_MARGIN / 2, drawY + fontSize / 2, DARKGREEN);
    }
}

void Renderer::applyRumbleEffect() {
    if (shakeIntensity <= 0) {
        return;
    }

    if (shakeIntensity > 0.01f) {
        shakeIntensity *= VISUAL_RUMBLE_DAMPENING;
        const int shakeIntensityInt = static_cast<int>(shakeIntensity);
        camera.offset.x = screenCenter.x + static_cast<float>(GetRandomValue(-shakeIntensityInt, shakeIntensityInt));
        camera.offset.y = screenCenter.y + static_cast<float>(GetRandomValue(-shakeIntensityInt, shakeIntensityInt));
    } else {
        shakeIntensity = 0.0f;
        camera.offset = screenCenter;
    }
}

// Main rendering function
void Renderer::draw() {
    applyRumbleEffect();

    BeginDrawing();
    ClearBackground(BLACK);

    // Render background if not in debug mode
    renderBackground();

    // Adjust y-position of camera
    camera.target.y = transformYCoordinate(world.getHiker().getRenderInformation().position.y) - 100.0f;
    camera.target.x = (world.getMaxX() + world.getMinX()) / 2.0f;

    BeginMode2D(camera);

    renderEntities();

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
    const floatType midOffsetY = static_cast<floatType>(GetScreenHeight()) -
                                 static_cast<floatType>(midgroundTex.height) * TEXTURE_MID_SCALE; // align lower border
    const floatType foreOffsetY =
        static_cast<floatType>(GetScreenHeight()) -
        static_cast<floatType>(foregroundTex.height) * TEXTURE_FORE_SCALE; // align lower border
    if (scrollingMid <= -static_cast<floatType>(midgroundTex.width) * TEXTURE_MID_SCALE) {
        scrollingMid = 0;
    }
    if (scrollingFore <= -static_cast<floatType>(foregroundTex.width) * TEXTURE_FORE_SCALE) {
        scrollingFore = 0;
    }
    // Draw midground image repeatedly
    drawBackgroundTextureRepeatedly(midgroundTex, scrollingMid, TEXTURE_MID_SCALE, midOffsetY);
    // Draw foreground image repeatedly
    drawBackgroundTextureRepeatedly(foregroundTex, scrollingFore, TEXTURE_FORE_SCALE, foreOffsetY);
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

int Renderer::floorToNearest(const int number, const int placeValue) const {
    return (number / placeValue) * placeValue;
}
void Renderer::toggleDebugMode() { this->debugMode = !this->debugMode; }

Vector Renderer::transformPosition(const Vector2 &vector, const Vector2 &offset) const {
    const auto transformedVector = transformPosition(vector);
    return Vector{transformedVector.x - offset.x, transformedVector.y - offset.y};
}
Vector Renderer::transformPosition(const Vector2 &vector) const {
    return Vector{vector.x, transformYCoordinate(vector.y)};
}

floatType Renderer::transformYCoordinate(const floatType yCoordinate) const {
    return -yCoordinate + static_cast<floatType>(GetScreenHeight());
}
