#include "Renderer.h"
#include "../../game/Game.hpp"
#include "raylib.h"

#include <cmath>
#include <memory>
#include <mutex>

constexpr Vector3 UP_DIR = {0, 1, 0};

Renderer::Renderer() {
    floatType leftBorder = world.getMinX();
    floatType rightBorder = world.getMaxX();

    // Calculate the world width
    floatType worldWidth = rightBorder - leftBorder;

    // Calculate the zoom level so that the world width fits the screen width
    auto screenWidth = static_cast<floatType>(GetScreenWidth());
    floatType zoom = screenWidth / worldWidth;

    // Calculate the visible width and height
    floatType visibleWidth = rightBorder - leftBorder;
    floatType visibleHeight =
        visibleWidth * (static_cast<floatType>(GetScreenHeight()) / static_cast<floatType>(GetScreenWidth()));

    // Calculate the center of the camera view based on the borders
    floatType centerX = (leftBorder + rightBorder) / 2.0f;
    floatType centerY = visibleHeight / 2.0f;

    // Initialize the camera
    camera.target = {centerX, centerY};
    camera.offset = {screenWidth / 2.0f, static_cast<floatType>(GetScreenHeight()) / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = zoom;

    regenerateGradientTexture();
}

Renderer::~Renderer() { CloseWindow(); }

// Function to render an entity
void Renderer::renderEntity(RenderedEntity &entity) {
    renderEntity(entity, entity.getRenderInformation().rotation.angular_offset);
}

void Renderer::renderEntity(RenderedEntity &entity, floatType rotation) {
    Texture2D texture = resourceManager.getTexture(entity.getRenderInformation().texture);
    Rectangle sourceRec = {0.0f, 0.0f, (floatType)texture.width, (floatType)texture.height}; // part of the texture used
    renderEntity(entity, rotation, texture, sourceRec);
}

void Renderer::renderEntity(RenderedEntity &entity, floatType rotation, Texture2D texture, Rectangle sourceRec) {
    auto info = entity.getRenderInformation();
    // Define the destination rectangle
    Rectangle destRec = {info.position.x + info.offset.x, info.position.y - info.offset.y, info.width, info.height};

    // Define the origin for rotation.
    Vector2 origin = {info.width / 2, info.height / 2};

    // Draw the texture if not in debug mode
    if (!Game::getInstance().debugMode) {
        DrawTexturePro(texture, sourceRec, destRec, origin, rotation, WHITE);
    } else {
        // Draw Rectangle for collision box, center with width and height
        DrawRectangleLines(static_cast<int>(info.position.x - info.width / 2),
                           static_cast<int>(info.position.y - info.height), static_cast<int>(info.width),
                           static_cast<int>(info.height), RED);
    }
}

void Renderer::animateEntity(RenderedEntity &entity) {
    auto info = entity.getRenderInformation();
    auto currentTime = static_cast<floatType>(GetTime());
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

void Renderer::renderAnimation(RenderedEntity &entity) {
    auto info = entity.getRenderInformation();
    // Get the texture and calculate frame width
    Texture2D texture = resourceManager.getTexture(info.texture);
    floatType width = static_cast<floatType>(texture.width) / static_cast<floatType>(info.animation.frames);
    floatType currentFrameWidth = width * static_cast<floatType>(info.animation.currentFrame);

    // Create source rectangle for the current frame
    Rectangle sourceRec = {currentFrameWidth, 0.0f, width, static_cast<floatType>(texture.height)};

    // Update render information for the entity
    info.width = width;
    info.height = static_cast<floatType>(texture.height);

    // Render the entity with the updated frame
    renderEntity(entity, info.rotation.angular_offset, texture, sourceRec);
}

void Renderer::renderHiker(RenderedEntity &hiker) {
    if (hiker.getRenderInformation().texture == "walk") {
        animateEntity(hiker);
    } else {
        renderEntity(hiker);
    }
}

void Renderer::debugRenderRock(RenderedEntity &entity) {
    // Draw Circle for collision box
    DrawCircleLines(static_cast<int>(entity.getRenderInformation().position.x),
                    static_cast<int>(entity.getRenderInformation().position.y), entity.getRenderInformation().width / 2,
                    RED);

    // Calculate the end points of the line to detect rotation
    floatType rotation = entity.getRenderInformation().rotation.angular_offset;
    floatType radius = entity.getRenderInformation().width / 2;
    int endX = static_cast<int>(entity.getRenderInformation().position.x + radius * std::cos(rotation));
    int endY = static_cast<int>(entity.getRenderInformation().position.y + radius * std::sin(rotation));

    // Draw the line
    DrawLine(static_cast<int>(entity.getRenderInformation().position.x),
             static_cast<int>(entity.getRenderInformation().position.y), endX, endY, RED);
}

void Renderer::renderMountain(Mountain &mountain, Color topColor, Color bottomColor) const {
    // Retrieve the relevant section of the mountain to be displayed
    IndexIntervalNew indexInterval = mountain.getIndexIntervalOfEntireMountain();
    // Get lower border of screen
    floatType lowerBorder =
        camera.target.y + (static_cast<floatType>(GetScreenHeight()) / (2.0f * camera.zoom)) + 100.0f;
    // Load the texture
    Texture2D texture = resourceManager.getTexture("mountain");
    Rectangle sourceRec = {0.0f, 0.0f, (floatType)texture.width, (floatType)texture.height};
    // Define origin and rotation
    Vector2 origin = {0.0f, 0.0f};
    floatType rotation = 0.0f;
    // Draw the mountain
    const int vertexOffset = 1;
    for (size_t i = indexInterval.startIndex; i < indexInterval.endIndex - vertexOffset; i += vertexOffset) {
        Position pos1 = mountain.getVertex(i);
        Position pos2 = mountain.getVertex(i + vertexOffset);
        // Render the mountain depending on debug mode
        if (Game::getInstance().debugMode) {
            // Draw the line
            DrawLine(static_cast<int>(pos1.x), static_cast<int>(pos1.y), static_cast<int>(pos2.x),
                     static_cast<int>(pos2.y), RED);
        } else {
            // Define destination rectangle (where to draw the texture, size of the texture in the destination)
            Rectangle destRec1 = {pos1.x, pos1.y, pos2.x - pos1.x,
                                  lowerBorder - pos1.y}; // todo make so it is not so pixelated
            DrawTexturePro(texture, sourceRec, destRec1, origin, rotation, WHITE);
        }
    }
}

void Renderer::renderEntities() {
    auto &hiker = world.getHiker();
    auto &rocks = world.getRocks();
    auto &destroyedRocks = world.getDestroyedRocks();
    auto &monster = world.getMonster();
    auto &mountain = world.getMountain();

    // Render hiker
    renderHiker(hiker);

    // Render rocks
    for (auto &rock : rocks) {
        renderEntity(rock);
    }

    // Render destroyed rocks, e.g. explosions
    for (auto &destroyedRock : destroyedRocks) {
        animateEntity(destroyedRock);
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

void Renderer::debugRenderEntities() {
    auto &hiker = world.getHiker();
    auto &rocks = world.getRocks();
    auto &monster = world.getMonster();
    auto &mountain = world.getMountain();
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
        renderEntity(*item);
        auto fontSize = 20;
        auto itemType = item->getRenderInformation().texture.c_str();
        auto centerX = static_cast<int>(item->getRenderInformation().position.x) - MeasureText(itemType, fontSize) / 2;
        DrawText(itemType, centerX, static_cast<int>(item->getRenderInformation().position.y), fontSize, GREEN);
        DrawCircleLines(static_cast<int>(item->getRenderInformation().position.x),
                        static_cast<int>(item->getRenderInformation().position.y), HIKER_ITEM_COLLECTION_RANGE, BLUE);
    }
    // Render mountain
    renderMountain(mountain, SKYBLUE, BLUE);
}

void Renderer::renderItemSlot(Inventory &inventory, int slotNumber, int startX, int startY) {
    if (!inventory.slotIsEmpty(slotNumber)) {
        auto textureName = inventory.getItem(slotNumber)->getRenderInformation().texture;
        auto texture = resourceManager.getTexture(textureName);
        DrawTexturePro(texture, {0, 0, (floatType)texture.width, (floatType)texture.height},
                       {static_cast<floatType>(startX) + static_cast<floatType>(slotNumber) * INVENTORY_SLOT_SIZE,
                        static_cast<floatType>(startY), INVENTORY_SLOT_SIZE, INVENTORY_SLOT_SIZE},
                       {0, 0}, 0, WHITE);
        size_t numberOfItems = inventory.getNumberOfItems(slotNumber);
        if (numberOfItems > 0) {
            DrawText(std::to_string(numberOfItems).c_str(),
                     static_cast<int>(startX) + slotNumber * static_cast<int>(INVENTORY_SLOT_SIZE) + 5,
                     static_cast<int>(startY) + 5, 20, WHITE);
        }
    }
}

void Renderer::renderInventory() {
    auto &inventory = world.getInventory();

    int inventoryWidth = static_cast<int>(inventory.getNumberOfSlots()) * static_cast<int>(INVENTORY_SLOT_SIZE);
    int startX = GetScreenWidth() - inventoryWidth - UI_MARGIN;
    int startY = GetScreenHeight() - static_cast<int>(INVENTORY_SLOT_SIZE) - UI_MARGIN;

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

void Renderer::renderHealthBar() {
    auto &hiker = world.getHiker();
    floatType health = static_cast<floatType>(hiker.getHealthPoints()) / HIKER_MAX_HEALTH;
    int screenWidth = GetScreenWidth();
    int startX = screenWidth - HEALTH_BAR_WIDTH - UI_MARGIN;
    int startY = UI_MARGIN;

    // Draw the health bar
    DrawRectangle(startX, startY, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT, Fade(RED, 0.5f));
    DrawRectangleLines(startX, startY, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT, WHITE);
    DrawRectangle(startX, startY, static_cast<int>(HEALTH_BAR_WIDTH * health), HEALTH_BAR_HEIGHT, GREEN);
}

void Renderer::renderScore() {
    int fontSize = 20;
    const char *scoreText;
    scoreText = (std::to_string(Game::getInstance().getScore() / 10) + "m").c_str();
    auto centerX = GetScreenWidth() - MeasureText(scoreText, fontSize) - UI_MARGIN;
    DrawText(scoreText, centerX, UI_MARGIN * 3, fontSize, WHITE);
}

// Main rendering function
void Renderer::draw() {
    BeginDrawing();
    ClearBackground(BLACK);

    // Render background if not in debug mode
    if (!Game::getInstance().debugMode)
        renderBackground();

    // Adjust y-position of camera
    camera.target.y = world.getHiker().getRenderInformation().position.y - 100.0f;
    camera.target.x = (world.getMaxX() + world.getMinX()) / 2.0f;

    BeginMode2D(camera);

    if (!Game::getInstance().debugMode)
        renderEntities();
    else {
        debugRenderEntities();
    }

    EndMode2D();
    renderInventory();
    renderHealthBar();
    renderScore();

    DrawFPS(0, 0);

    EndDrawing();
}

void Renderer::renderBackground() {
    DrawTexture(gradient_texture_background, 0, 0, WHITE);

    // Get the textures
    Texture2D midgroundTex = resourceManager.getTexture("midground");
    Texture2D foregroundTex = resourceManager.getTexture("foreground");

    scrolling_mid -= 0.25f;
    scrolling_fore -= 0.5f;

    floatType midScale = 5;  // scale of texture
    floatType foreScale = 7; // scale of texture

    floatType midOffsetY = static_cast<floatType>(GetScreenHeight()) -
                           static_cast<floatType>(midgroundTex.height) * midScale; // align lower border
    floatType foreOffsetY = static_cast<floatType>(GetScreenHeight()) -
                            static_cast<floatType>(foregroundTex.height) * foreScale; // align lower border

    if (scrolling_mid <= -static_cast<floatType>(midgroundTex.width) * midScale)
        scrolling_mid = 0;
    if (scrolling_fore <= -static_cast<floatType>(foregroundTex.width) * foreScale)
        scrolling_fore = 0;

    // Draw midground image repeatedly
    drawBackgroundTextureRepeatedly(midgroundTex, scrolling_mid, midScale, midOffsetY);

    // Draw foreground image repeatedly
    drawBackgroundTextureRepeatedly(foregroundTex, scrolling_fore, foreScale, foreOffsetY);
}

void Renderer::drawBackgroundTextureRepeatedly(Texture2D texture2D, floatType scrolling, floatType scale,
                                               floatType offsetY) {

    DrawTextureEx(texture2D, {scrolling, offsetY}, 0.0f, scale, WHITE);
    DrawTextureEx(texture2D, {static_cast<floatType>(texture2D.width) * scale + scrolling, offsetY}, 0.0f, scale,
                  WHITE);
    DrawTextureEx(texture2D, {static_cast<floatType>(texture2D.width) * scale * 2 + scrolling, offsetY}, 0.0f, scale,
                  WHITE);
}

void Renderer::regenerateGradientTexture() {
    UnloadTexture(gradient_texture_background); // TODO necessary?
    Image verticalGradient = GenImageGradientV(GetScreenWidth(), GetScreenHeight(), BLUE, WHITE);
    gradient_texture_background = LoadTextureFromImage(verticalGradient);
    UnloadImage(verticalGradient);
}
