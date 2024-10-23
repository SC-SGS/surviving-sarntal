//
// Created by six on 7/19/24.
//

#include "TerrainRenderer.h"
#include "poly2tri/poly2tri.h"

TerrainRenderer::TerrainRenderer(Camera2D &camera, GameConstants &gameConstants, ResourceManager &resourceManager)
    : camera(camera), gameConstants(gameConstants) {
    mountainTexture = resourceManager.getTexture("mountain");
    // Alle Optionen: TEXTURE_WRAP_REPEAT, TEXTURE_WRAP_CLAMP, TEXTURE_WRAP_MIRROR_REPEAT
    SetTextureWrap(mountainTexture, TEXTURE_WRAP_MIRROR_REPEAT);
}

void TerrainRenderer::renderMountain(const Terrain &terrain, bool debug) {
    floatType newMinX = static_cast<float>(floor(terrain.getLeftBorder())) * graphics::UNIT_TO_PIXEL_RATIO;
    floatType newMaxX = static_cast<float>(ceil(terrain.getRightBorder())) * graphics::UNIT_TO_PIXEL_RATIO;

    // Only update if the terrain or borders have changed
    bool shouldUpdate = newMinX != minX || newMaxX != maxX || verticesNeedUpdate;
    if (shouldUpdate) {
        minX = newMinX;
        maxX = newMaxX;
        updateVertices(terrain);
        verticesNeedUpdate = false; // Reset flag after update
    }

    if (!debug) {
        // Bind the texture and draw the triangulated mesh
        rlSetTexture(mountainTexture.id);
        drawMountainMesh();
        drawSurface(terrain);
        rlSetTexture(0);
    } else {
        debugRenderMountain();
    }
}

void TerrainRenderer::updateVertices(const Terrain &terrain) {
    std::vector<std::shared_ptr<StaticPolyline>> groundComponents = terrain.getPolyRepresentationOfGroundRendering();

    // Clear previous vertices and colors
    vertices.clear();
    colors.clear();

    for (const auto &ground : groundComponents) {
        std::vector<p2t::Point *> polylinePoints = createPolylinePoints(ground);

        // Triangulate the closed shape
        p2t::CDT cdt(polylinePoints);
        cdt.Triangulate();
        std::vector<p2t::Triangle *> triangles = cdt.GetTriangles();

        for (auto *triangle : triangles) {
            for (int i = 2; i >= 0; i--) {
                p2t::Point *point = triangle->GetPoint(i);
                Vector2 vertex = {static_cast<float>(point->x), static_cast<float>(point->y)};
                vertices.push_back(vertex);
            }
            p2t::Point *point = triangle->GetPoint(0);
            Vector2 vertex = {static_cast<float>(point->x), static_cast<float>(point->y)};
            vertices.push_back(vertex);
        }
    }
}

std::vector<p2t::Point *> TerrainRenderer::createPolylinePoints(const std::shared_ptr<StaticPolyline> &ground) const {
    std::vector<p2t::Point *> polylinePoints;
    floatType lowestPoint = std::numeric_limits<floatType>::min(); // Initialize to a high (low) value

    // Prepare polyline points and find the lowest y-coordinate in one loop
    for (Vector point : ground->getPoints()) {
        Vector pointTransformed = GraphicsUtil::transformPosition(Vector2(point));
        polylinePoints.push_back(new p2t::Point(pointTransformed.x, pointTransformed.y));

        // Track the lowest y-coordinate
        if (pointTransformed.y > lowestPoint) { // Assuming higher y means lower on screen
            lowestPoint = pointTransformed.y;
        }
    }

    floatType screenVisibleHeight =
        static_cast<float>(GetScreenHeight()) *
        (static_cast<float>(GetScreenWidth()) / static_cast<float>(gameConstants.visualConstants.worldSize));
    floatType offset =
        static_cast<float>(gameConstants.visualConstants.cameraToHikerOffset) * graphics::UNIT_TO_PIXEL_RATIO;
    floatType lowerBorder = lowestPoint + screenVisibleHeight / 2 + offset;

    // Close the shape with the lower border
    polylinePoints.push_back(new p2t::Point(polylinePoints.back()->x, lowerBorder));
    polylinePoints.push_back(new p2t::Point(polylinePoints.front()->x, lowerBorder));

    return polylinePoints;
}

void TerrainRenderer::debugRenderMountain() {
    rlBegin(RL_LINES);
    rlColor3f(1.0f, 0.0f, 0.0f);
    for (int i = 0; i < vertices.size() - 2; i += 4) {
        rlVertex2f(vertices[i].x, vertices[i].y);
        rlVertex2f(vertices[i + 1].x, vertices[i + 1].y);

        rlVertex2f(vertices[i + 1].x, vertices[i + 1].y);
        rlVertex2f(vertices[i + 2].x, vertices[i + 2].y);

        rlVertex2f(vertices[i + 2].x, vertices[i + 2].y);
        rlVertex2f(vertices[i].x, vertices[i].y);
    }
    rlEnd();
}

void TerrainRenderer::drawMountainMesh() const {
    rlBegin(RL_QUADS);
    rlColor4ub(255, 255, 255, 255); // White color to avoid tinting the texture

    for (auto vertex : vertices) {
        // Compute texture coordinates (assuming continuous texture wrapping)
        float texX = vertex.x / static_cast<float>(mountainTexture.width);
        float texY = vertex.y / static_cast<float>(mountainTexture.height);

        rlTexCoord2f(texX, texY);       // Texture mapping
        rlVertex2f(vertex.x, vertex.y); // Vertex position
    }
    rlEnd();
}

void TerrainRenderer::drawSurface(const Terrain &terrain) const {
    rlSetLineWidth(10);
    for (const auto &ground : terrain.getPolyRepresentationOfGroundRendering()) {
        rlBegin(RL_LINES);
        rlColor3f(0.0f, 0.5f, 0.0f);
        for (int i = 0; i < ground->getPoints().size() - 1; i++) {
            Vector point1 = GraphicsUtil::transformPosition(Vector2(ground->getPoints()[i]));
            Vector point2 = GraphicsUtil::transformPosition(Vector2(ground->getPoints()[i + 1]));
            rlVertex2f(point1.x, point1.y);
            rlVertex2f(point2.x, point2.y);
        }
        rlEnd();
    }
}

void TerrainRenderer::reset() { verticesNeedUpdate = true; }
