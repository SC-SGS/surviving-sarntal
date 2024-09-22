//
// Created by felix-roehr on 9/7/24.
//

#include "GenerationRenderer.hpp"
#include "rlgl.h"
GenerationRenderer::GenerationRenderer(HikerConstants &hikerConstants, TerrainConstants &terrainConstants,
                                       ResourceManager &resourceManager)
    : terrainConstants(terrainConstants), hikerConstants(hikerConstants), resourceManager(resourceManager) {}

const void GenerationRenderer::render( // NOLINT [readability-function-size,-warnings-as-errors]
    const std::shared_ptr<Ground> &newGround, const std::shared_ptr<StaticPolyline> &groundPolyRepresentation,
    Vector &newPoint, TerrainPhase &phase, floatType minAngle, floatType maxAngle) {
    Vector oldPoint = newGround->getBasePoints()->getPoints().at(
        std::max(0, static_cast<int>(newGround->getBasePoints()->getPoints().size() - 2)));
    Camera2D camera = this->setUpRendering();
    BeginDrawing();
    // ClearBackground(BLACK);
    this->renderBackground();
    BeginMode2D(camera);
    Vector delta = {0.5f, 0.5f};
    Vector2 basePointRendering =
        GenerationRenderer::transformPosition(newGround->getBasePoints()->getStartPoint() + delta);
    DrawRectangle(static_cast<int>(basePointRendering.x),
                  static_cast<int>(basePointRendering.y - this->hikerConstants.hikerHeight),
                  static_cast<int>(this->hikerConstants.hikerWidth), static_cast<int>(this->hikerConstants.hikerHeight),
                  RED);
    rlBegin(RL_LINES);
    rlColor3f(1.0f, 0.0f, 0.0f);
    for (int i = 0; i < groundPolyRepresentation->getPoints().size() - 2; i++) {
        Vector point = groundPolyRepresentation->getPoints().at(i);
        Vector nextPoint = groundPolyRepresentation->getPoints().at(i + 1);

        Vector2 renderingPoint = GenerationRenderer::transformPosition(point);
        Vector2 renderingToPoint = GenerationRenderer::transformPosition(nextPoint);

        rlVertex2f(renderingPoint.x, renderingPoint.y);
        rlVertex2f(renderingToPoint.x, renderingToPoint.y);
    }
    rlEnd();
    const std::shared_ptr<StaticPolyline> basePoints = newGround->getBasePoints();
    for (Vector point : basePoints->getPoints()) {
        Vector2 renderingPoint = GenerationRenderer::transformPosition(point);
        DrawCircle(static_cast<int>(renderingPoint.x), static_cast<int>(renderingPoint.y), 0.25f, WHITE);
    }
    Vector minPoint = oldPoint + phase.avgDirection.rotateByAngle(minAngle);
    Vector maxPoint = oldPoint + phase.avgDirection.rotateByAngle(maxAngle);
    Vector2 oldPointRendering = GenerationRenderer::transformPosition(oldPoint);
    Vector2 minPointRendering = GenerationRenderer::transformPosition(minPoint);
    Vector2 maxPointRendering = GenerationRenderer::transformPosition(maxPoint);
    DrawLineEx(oldPointRendering, minPointRendering, 0.15f, RED);
    DrawLineEx(oldPointRendering, maxPointRendering, 0.15f, RED);
    Vector2 newPointRendering = GenerationRenderer::transformPosition(newPoint);
    DrawLineEx(oldPointRendering, newPointRendering, 0.15f, RED);
    EndMode2D();
    EndDrawing();
}

Camera2D GenerationRenderer::setUpRendering() const {
    const floatType leftBorder = -0.02f * this->terrainConstants.biomeWidth;
    const floatType rightBorder = 1.02f * this->terrainConstants.biomeWidth;

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
    const floatType centerY = 0;

    const Vector2 screenCenter = {static_cast<float>(GetScreenWidth()) / 2, static_cast<float>(GetScreenHeight()) / 2};

    Camera2D camera;
    camera.target = {centerX, centerY};
    camera.offset = screenCenter;
    camera.rotation = 0.0f;
    camera.zoom = zoom;

    return camera;
}

Vector2 GenerationRenderer::transformPosition(Vector position) {
    floatType xValue = position.x;
    floatType yValue = -position.y;
    return {xValue, yValue};
    // return {position.x, -position.y + static_cast<floatType>(GetScreenHeight())/camera.zoom};
}

void GenerationRenderer::renderBackground() {
    Texture2D backgroundTexture = resourceManager.getTexture("terrainGenerationBackground");
    const Rectangle sourceRec = {0.0f, 0.0f, graphics::SCREEN_WIDTH_IN_PIXEL, graphics::SCREEN_HEIGHT_IN_PIXEL};
    const Rectangle destRec = {0.0f, 0.0f, graphics::SCREEN_WIDTH_IN_PIXEL, graphics::SCREEN_HEIGHT_IN_PIXEL};

    DrawTexturePro(backgroundTexture, sourceRec, destRec, {0, 0}, 0, WHITE);
}
