//
// Created by Anietta Weckauff on 09.08.24.
//

#include "PolygonRenderer.h"
#include "raymath.h"
#include "rlgl.h"
PolygonRenderer::PolygonRenderer(ResourceManager &resourceManager) : resourceManager(resourceManager){};

void PolygonRenderer::renderPolygon(Polygon &polygon) {
    std::vector<Vector> vertices = polygon.getVertices();
    Vector centroid = polygon.calculateCentroid();
    Vector2 centroid2 = {centroid.x, centroid.y};

    vertices.push_back(vertices[0]);

    for (size_t i = 0; i < vertices.size() - 1; ++i) {
        DrawTriangle(centroid2, {vertices[i].x, vertices[i].y}, {vertices[i + 1].x, vertices[i + 1].y}, GRAY);
    }
}

void PolygonRenderer::renderPolygonOutline(Polygon &polygon) {
    std::vector<Vector> vertices = polygon.getVertices();
    Vector centroid = polygon.calculateCentroid();

    DrawCircle(static_cast<int>(centroid.x), static_cast<int>(centroid.y), 5.0, WHITE);

    for (size_t i = 0; i < vertices.size(); ++i) {
        Vector point1 = vertices[i];
        Vector point2 = vertices[(i + 1) % vertices.size()];
        DrawLineEx({point1.x, point1.y}, {point2.x, point2.y}, 3, WHITE);
        DrawLine(static_cast<int>(point1.x), static_cast<int>(point1.y), static_cast<int>(centroid.x),
                 static_cast<int>(centroid.y), WHITE);
    }
}

void PolygonRenderer::renderTexturedPolygon(Polygon &polygon) {
    Texture2D texture = resourceManager.getTexture("rockRectangle");
    std::vector<Vector> vertices = polygon.getVertices();

    std::vector<Vector2> raylibVertices = polygon.getRaylibVertices();

    // render outline
    for (size_t i = 0; i < raylibVertices.size(); ++i) {
        Vector2 point1 = raylibVertices[i];
        Vector2 point2 = raylibVertices[(i + 1) % raylibVertices.size()];
        DrawLineEx({point1.x, point1.y}, {point2.x, point2.y}, 4, BLACK);
    }

    std::vector<Vector2> texCoords = polygon.getTextureCoordinates();

    Vector centroid = polygon.calculateCenter();
    Vector2 center = {centroid.x, centroid.y};

    // render textured polygon
    this->drawTexturePoly(texture, center, raylibVertices.data(), texCoords.data(),
                          static_cast<int>(raylibVertices.size()), WHITE);
}

void PolygonRenderer::drawTexturePoly(Texture2D texture, Vector2 center, Vector2 *points, Vector2 *texCoords,
                                      int pointCount, Color tint) {
    rlSetTexture(texture.id);

    rlBegin(RL_QUADS);

    rlColor4ub(tint.r, tint.g, tint.b, tint.a);

    for (int i = 0; i < pointCount - 1; i++) {
        rlTexCoord2f(0.5f, 0.5f);
        rlVertex2f(center.x, center.y);

        rlTexCoord2f(texCoords[i].x, texCoords[i].y);
        rlVertex2f(points[i].x, points[i].y);

        rlTexCoord2f(texCoords[i + 1].x, texCoords[i + 1].y);
        rlVertex2f(points[i + 1].x, points[i + 1].y);

        rlTexCoord2f(texCoords[i + 1].x, texCoords[i + 1].y);
        rlVertex2f(points[i + 1].x, points[i + 1].y);
    }
    rlEnd();

    rlSetTexture(0);
}
