#include "Renderer.h"
#include "../../entities/Hiker.h"
#include "../../entities/RenderedEntity.h"
#include "../../entities/Rock.h"
#include "../../utils/game_constants.h"
#include "raylib.h"

constexpr Vector3 UP_DIR = {0, 1, 0};

// Function to render an entity
void Renderer::renderEntity(const RenderInformation &info) { renderEntity(info, 0.0f); }

void Renderer::renderEntity(const RenderInformation &info, float rotation) {
    Texture2D texture = resourceManager.getTexture(info.texture);
    Rectangle sourceRec = {0.0f, 0.0f, (float)texture.width, (float)texture.height}; // part of the texture used
    renderEntity(info, rotation, texture, sourceRec);
}

void Renderer::renderEntity(const RenderInformation &info, float rotation, Texture2D texture, Rectangle sourceRec) {
    DrawBillboardPro(camera, texture, sourceRec, Vector3{info.position.x, info.position.y + info.height / 2, 0}, UP_DIR,
                     Vector2{static_cast<float>(info.width), static_cast<float>(info.height)}, Vector2{0.0f, 0.0f},
                     rotation, WHITE);
}

static float currentFrame = 0;
static float framesSpeed = 5;
const float framesCounter = 4.0f;
static float lastTime = 0;
void Renderer::renderHiker(RenderInformation &hikerInfo) {
    if (hikerInfo.texture == "walk") {
        animateHiker(hikerInfo);
    } else {
        renderEntity(hikerInfo);
    }
}

void Renderer::animateHiker(RenderInformation &hikerInfo) {
    Texture2D texture = resourceManager.getTexture(hikerInfo.texture);

    float width = (float)texture.width / framesCounter;
    float currentFrameWidth = width * currentFrame;

    if (GetTime() >= lastTime + 1.0f / framesSpeed) {
        lastTime = static_cast<float>(GetTime());
        currentFrame += 1;
        if (currentFrame >= framesCounter) {
            currentFrame = 0;
        }
    }

    Rectangle sourceRec = {currentFrameWidth, 0.0f, width, (float)texture.height};
    hikerInfo = {hikerInfo.position, width / 5, static_cast<float>(texture.height) / 10, hikerInfo.rotation,
                 hikerInfo.texture};
    renderEntity(hikerInfo, 0, texture, sourceRec);
}

void Renderer::renderRock(const RenderInformation &rockInfo) {
    renderEntity(rockInfo, rockInfo.rotation.angular_offset);
}

// Main rendering function
void Renderer::render(Hiker hiker, RockClass rocks[], int numRocks) {
    BeginDrawing();
    ClearBackground(BLACK);

    // Render background
    renderBackground();

    BeginMode3D(camera);

    // DrawGrid(1000, 10.0f);

    // Render hiker
    RenderInformation hikerInfo = hiker.getRenderInformation();
    renderHiker(hikerInfo);

    // Render rocks
    for (int i = 0; i < numRocks; ++i) {
        RenderInformation rockInfo = rocks[i].getRenderInformation();
        renderRock(rockInfo);
    }

    EndMode3D();

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
