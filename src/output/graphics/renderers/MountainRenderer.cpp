//
// Created by six on 7/19/24.
//

#include "MountainRenderer.h"

MountainRenderer::MountainRenderer(Camera2D &camera, GameConstants gameConstants)
    : camera(camera), gameConstants(gameConstants) {}

void MountainRenderer::renderMountain(const Terrain &terrain, Color topColor, Color bottomColor, bool debug) {
    StaticPolyline *ground = terrain.getPolyRepresentationOfGroundRendering();
    for (Vector point : ground->getPoints()) {
        Vector pointTransformed = GraphicsUtil::transformPosition(Vector2(point));
        DrawCircle(static_cast<int>(pointTransformed.x), static_cast<int>(pointTransformed.y), 2.0f, RED);
    }
    // updateVertices(terrain, topColor, bottomColor);
    //    if (!debug) {
    //        // drawMountainMesh();
    //        // drawMountainBase(bottomColor);
    //    } else {
    //        // debugRenderMountain();
    //    };
}

void MountainRenderer::debugRenderMountain() {
    rlBegin(RL_LINES);
    rlColor3f(1.0f, 0.0f, 0.0f);
    for (int i = 0; i < vertices.size() - 2; i += 2) {
        rlVertex2f(vertices[i].x, vertices[i].y);
        rlVertex2f(vertices[i + 2].x, vertices[i + 2].y);
    }
    rlEnd();
}

void MountainRenderer::drawMountainMesh() const {
    rlBegin(RL_TRIANGLES);
    for (int index : indices) {
        rlColor3f(colors[index].x, colors[index].y, colors[index].z);
        rlVertex2f(vertices[index].x, vertices[index].y);
    }
    rlEnd();
}

void MountainRenderer::drawMountainBase(Color bottomColor) const {
    auto normalizedBottomColor = normalizeColor(bottomColor);
    rlBegin(RL_TRIANGLES);
    for (int i = 1; i < indices.size() - 2; i += 2) {
        drawBaseTriangle(i, normalizedBottomColor);
    }
    rlEnd();
}

void MountainRenderer::drawBaseTriangle(int index, Vector3 normalizedColor) const {
    rlColor3f(normalizedColor.x, normalizedColor.y, normalizedColor.z);
    rlVertex2f(vertices[index].x, vertices[index].y);
    rlVertex2f(vertices[index].x, calculateLowerBorder());
    rlVertex2f(vertices[index + 2].x, vertices[index + 2].y);

    rlVertex2f(vertices[index].x, calculateLowerBorder());
    rlVertex2f(vertices[index + 2].x, calculateLowerBorder());
    rlVertex2f(vertices[index + 2].x, vertices[index + 2].y);
}

void MountainRenderer::updateVertices(const Terrain &terrain, Color topColor, Color bottomColor) {
    floatType newMinX = static_cast<float>(floor(terrain.getLeftBorder())) * graphics::UNIT_TO_PIXEL_RATIO;
    floatType newMaxX = static_cast<float>(ceil(terrain.getRightBorder())) * graphics::UNIT_TO_PIXEL_RATIO;

    if (newMinX != minX || newMaxX != maxX) {
        removeOutOfBoundsVerticesAndColors(newMinX);
        updateBorders(newMinX, newMaxX);
        addNewVerticesAndColors(terrain, topColor, bottomColor);
        createTriangles();
    }
}

void MountainRenderer::removeOutOfBoundsVerticesAndColors(floatType newMinX) {
    auto iterator = std::find_if(vertices.begin(), vertices.end(),
                                 [newMinX](const Vector2 &vertex) { return vertex.x >= static_cast<float>(newMinX); });

    if (iterator != vertices.begin()) {
        int indexToRemove = static_cast<int>(std::distance(vertices.begin(), iterator));
        vertices.erase(vertices.begin(), iterator);
        colors.erase(colors.begin(), colors.begin() + indexToRemove);
        updateIndices(indexToRemove);
    }
}

void MountainRenderer::updateIndices(int indexToRemove) {
    indices.erase(
        std::remove_if(indices.begin(), indices.end(), [indexToRemove](int index) { return index < indexToRemove; }),
        indices.end());

    std::for_each(indices.begin(), indices.end(), [indexToRemove](int &index) { index -= indexToRemove; });
}

void MountainRenderer::updateBorders(floatType newMinX, floatType newMaxX) {
    minX = newMinX;
    maxX = newMaxX;
}

void MountainRenderer::addNewVerticesAndColors(const Terrain &terrain, Color topColor, Color bottomColor) {
    int step = gameConstants.visualConstants.mountainResolution;
    floatType startX = vertices.empty() ? minX : vertices.back().x + static_cast<float>(step);
    updateVerticesAndColors(terrain, topColor, bottomColor, startX);
}

floatType MountainRenderer::calculateLowerBorder() const {
    return camera.target.y + (static_cast<floatType>(GetScreenHeight()) / (2.0f * camera.zoom)) +
           static_cast<floatType>(gameConstants.visualConstants.cameraToHikerOffset);
}

void MountainRenderer::updateVerticesAndColors(const Terrain &terrain, Color topColor, Color bottomColor,
                                               floatType startX) {
    auto step = static_cast<floatType>(gameConstants.visualConstants.mountainResolution);
    for (floatType xPos = startX; xPos <= maxX; xPos += step) { // NOLINT(*-flp30-c)
        addVertexAndColor(terrain, topColor, bottomColor, xPos);
    }
}

void MountainRenderer::addVertexAndColor(const Terrain &terrain, Color topColor, Color bottomColor, floatType xPos) {
    // floatType yPos = GraphicsUtil::transformYCoordinate(
    //         terrain.calculateYPos(static_cast<floatType>(xPos) / graphics::UNIT_TO_PIXEL_RATIO));
    //  vertices.emplace_back(Vector2{static_cast<float>(xPos), static_cast<float>(yPos)});
    //  colors.push_back(normalizeColor(topColor));
    //  vertices.emplace_back(Vector2{static_cast<float>(xPos),
    //                                yPos + static_cast<float>(gameConstants.visualConstants.mountainGradientHeight)});
    //  colors.push_back(normalizeColor(bottomColor));
}

void MountainRenderer::createTriangles() {
    indices.clear();
    for (int i = 0; i < vertices.size() - 3; i += 2) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);

        indices.push_back(i + 1);
        indices.push_back(i + 3);
        indices.push_back(i + 2);
    }
}

Vector3 MountainRenderer::normalizeColor(const Color &color) const {
    return Vector3{static_cast<floatType>(color.r) / 255.0f, static_cast<floatType>(color.g) / 255.0f,
                   static_cast<floatType>(color.b) / 255.0f};
}
