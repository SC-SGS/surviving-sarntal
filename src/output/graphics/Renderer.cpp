#include "Renderer.h"
#include "raylib.h"

#include <memory>
#include <mutex>

constexpr Vector3 UP_DIR = {0, 1, 0};

Renderer::Renderer() {
    float leftBorder = world.getMinX();
    float rightBorder = world.getMaxX();

    // Calculate the world width
    float worldWidth = rightBorder - leftBorder;

    // Calculate the zoom level so that the world width fits the screen width
    auto screenWidth = static_cast<float>(GetScreenWidth());
    float zoom = screenWidth / worldWidth;

    // Calculate the visible width and height
    float visibleWidth = rightBorder - leftBorder;
    float visibleHeight = visibleWidth * (static_cast<float>(GetScreenHeight()) / static_cast<float>(GetScreenWidth()));

    // Calculate the center of the camera view based on the borders
    float centerX = (leftBorder + rightBorder) / 2.0f;
    float centerY = visibleHeight / 2.0f;

    // Initialize the camera
    camera.target = {centerX, centerY};
    camera.offset = {screenWidth / 2.0f, static_cast<float>(GetScreenHeight()) / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = zoom;

    regenerateGradientTexture();
    std::cout << "Renderer initialized" << std::endl;
}

Renderer::~Renderer() {
    CloseWindow();
    std::cout << "Renderer destroyed" << std::endl;
}

// Function to render an entity
void Renderer::renderEntity(RenderedEntity &entity) { renderEntity(entity, 0.0f); }

void Renderer::renderEntity(RenderedEntity &entity, float rotation) {
    Texture2D texture = resourceManager.getTexture(entity.getRenderInformation().texture);
    Rectangle sourceRec = {0.0f, 0.0f, (float)texture.width, (float)texture.height}; // part of the texture used
    renderEntity(entity, rotation, texture, sourceRec);
}

void Renderer::renderEntity(RenderedEntity &entity, float rotation, Texture2D texture, Rectangle sourceRec) {
    auto info = entity.getRenderInformation();
    // Define the destination rectangle
    Rectangle destRec = {info.position.x + info.offset.x, info.position.y - info.offset.y, info.width, info.height};

    // Define the origin for rotation.
    Vector2 origin = {info.width / 2, info.height / 2};

    // DrawCircle(static_cast<int>(entity.getPosition().x), static_cast<int>(entity.getPosition().y), 100.f,
    // BLACK); // todo @daniel pls fix this
    // Draw the texture
    DrawTexturePro(texture, sourceRec, destRec, origin, rotation, WHITE);
}

void Renderer::animateEntity(RenderedEntity &entity) {
    auto info = entity.getRenderInformation();
    auto currentTime = static_cast<float>(GetTime());
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
    float width = static_cast<float>(texture.width) / static_cast<float>(info.animation.frames);
    float currentFrameWidth = width * static_cast<float>(info.animation.currentFrame);

    // Create source rectangle for the current frame
    Rectangle sourceRec = {currentFrameWidth, 0.0f, width, static_cast<float>(texture.height)};

    // Update render information for the entity
    info.width = width;
    info.height = static_cast<float>(texture.height);

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

void Renderer::renderRock(RenderedEntity &entity) {
    renderEntity(entity, entity.getRenderInformation().rotation.angular_offset);
}

void Renderer::renderMountain(MountainClass &mountain, Color topColor, Color bottomColor) const {
    // Retrieve the relevant section of the mountain to be displayed
    IndexIntervalNew indexInterval = mountain.getIndexIntervalOfEntireMountain();

    // Get lower border of screen
    float lowerBorder = camera.target.y + (static_cast<float>(GetScreenHeight()) / (2.0f * camera.zoom)) + 100.0f;

    // Load the texture
    Texture2D texture = resourceManager.getTexture("mountain");
    Rectangle sourceRec = {0.0f, 0.0f, (float)texture.width, (float)texture.height};

    // Define origin and rotation
    Vector2 origin = {0.0f, 0.0f};
    float rotation = 0.0f;

    // Draw the mountain
    const int vertexOffset = 1;
    for (size_t i = indexInterval.startIndex; i < indexInterval.endIndex - vertexOffset; i += vertexOffset) {
        Position pos1 = mountain.getVertex(i);
        Position pos2 = mountain.getVertex(i + vertexOffset);
        // Define destination rectangle (where to draw the texture, size of the texture in the destination)
        Rectangle destRec1 = {pos1.x, pos1.y, pos2.x - pos1.x,
                              lowerBorder - pos1.y}; // todo make so it is not so pixelated

        DrawTexturePro(texture, sourceRec, destRec1, origin, rotation, WHITE);
    }
}

void Renderer::renderEntities() {
    auto &hiker = world.getHiker();
    auto &rocks = world.getRocks();
    auto &monster = world.getMonster();
    auto &mountain = world.getMountain();

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

    // Render mountain
    renderMountain(mountain, SKYBLUE, BLUE);
}

void Renderer::renderItemSlot(Inventory &inventory, int slotNumber, int startX, int startY) {
    auto itemType = inventory.getItem(slotNumber);
    if (itemType == ItemType::NO_ITEM) {
        return;
    }
    auto itemInfo = Item::getItemInformation(itemType);
    auto texture = resourceManager.getTexture(itemInfo.name);
    DrawTexturePro(texture, {0, 0, (float)texture.width, (float)texture.height},
                   {static_cast<floatType>(startX) + static_cast<floatType>(slotNumber) * INVENTORY_SLOT_SIZE,
                    static_cast<float>(startY), INVENTORY_SLOT_SIZE, INVENTORY_SLOT_SIZE},
                   {0, 0}, 0, WHITE);
}

void Renderer::renderInventory() {
    auto &inventory = world.getInventory();

    int inventoryWidth = static_cast<int>(inventory.getSlotCount()) * static_cast<int>(INVENTORY_SLOT_SIZE);
    int startX = GetScreenWidth() - inventoryWidth - UI_MARGIN;
    int startY = GetScreenHeight() - static_cast<int>(INVENTORY_SLOT_SIZE) - UI_MARGIN;

    for (int i = 0; i < inventory.getSlotCount(); i++) {
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
    float health = static_cast<float>(hiker.getHealthPoints()) / HIKER_MAX_HEALTH;
    int screenWidth = GetScreenWidth();
    int startX = screenWidth - HEALTH_BAR_WIDTH - UI_MARGIN;
    int startY = UI_MARGIN;

    // Draw the health bar
    DrawRectangle(startX, startY, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT, Fade(RED, 0.5f));
    DrawRectangleLines(startX, startY, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT, WHITE);
    DrawRectangle(startX, startY, static_cast<int>(HEALTH_BAR_WIDTH * health), HEALTH_BAR_HEIGHT, GREEN);
}

// Main rendering function
void Renderer::draw() {
    BeginDrawing();
    ClearBackground(BLACK);

    DrawGrid(1000, 20);

    // Render background
    renderBackground();

    // Adjust y-position of camera
    camera.target.y = world.getHiker().getRenderInformation().position.y - 100.0f;
    camera.target.x = (world.getMaxX() + world.getMinX()) / 2.0f;

    BeginMode2D(camera);

    renderEntities();

    EndMode2D();
    renderInventory();
    renderHealthBar();

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

    float midScale = 5;  // scale of texture
    float foreScale = 7; // scale of texture

    float midOffsetY = static_cast<float>(GetScreenHeight()) -
                       static_cast<float>(midgroundTex.height) * midScale; // align lower border
    float foreOffsetY = static_cast<float>(GetScreenHeight()) -
                        static_cast<float>(foregroundTex.height) * foreScale; // align lower border

    if (scrolling_mid <= -static_cast<float>(midgroundTex.width) * midScale)
        scrolling_mid = 0;
    if (scrolling_fore <= -static_cast<float>(foregroundTex.width) * foreScale)
        scrolling_fore = 0;

    // Draw midground image repeatedly
    drawBackgroundTextureRepeatedly(midgroundTex, scrolling_mid, midScale, midOffsetY);

    // Draw foreground image repeatedly
    drawBackgroundTextureRepeatedly(foregroundTex, scrolling_fore, foreScale, foreOffsetY);
}

void Renderer::drawBackgroundTextureRepeatedly(Texture2D texture2D, float scrolling, float scale, float offsetY) {

    DrawTextureEx(texture2D, {scrolling, offsetY}, 0.0f, scale, WHITE);
    DrawTextureEx(texture2D, {static_cast<float>(texture2D.width) * scale + scrolling, offsetY}, 0.0f, scale, WHITE);
    DrawTextureEx(texture2D, {static_cast<float>(texture2D.width) * scale * 2 + scrolling, offsetY}, 0.0f, scale,
                  WHITE);
}

void Renderer::regenerateGradientTexture() {
    UnloadTexture(gradient_texture_background); // TODO necessary?
    Image verticalGradient = GenImageGradientV(GetScreenWidth(), GetScreenHeight(), BLUE, WHITE);
    gradient_texture_background = LoadTextureFromImage(verticalGradient);
    UnloadImage(verticalGradient);
}
