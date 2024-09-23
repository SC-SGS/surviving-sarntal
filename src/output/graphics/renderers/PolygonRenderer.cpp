//
// Created by Anietta Weckauff on 09.08.24.
//

#include "PolygonRenderer.h"

#include "../../../../cmake-build-debug/_deps/raylib-src/src/raymath.h"
#include "../../../../cmake-build-debug/_deps/raylib-src/src/rlgl.h"
#include "../../../spawner/PolygonGenerator.h"
#include "../GraphicsUtil.h"
PolygonRenderer::PolygonRenderer(ResourceManager &resourceManager) : resourceManager(resourceManager){};

void PolygonRenderer::renderPolygon(const DynamicPolygon &polygon) const {
    std::vector<Vector> vertices = polygon.getWorldSpaceVertices();
    const Vector centroid = GraphicsUtil::transformPosition(Vector2(polygon.getPosition()));
    const Vector2 centroid2 = {centroid.x, centroid.y};

    vertices.push_back(vertices[0]);

    for (size_t i = 0; i < vertices.size() - 1; ++i) {
        const Vector vertex1 = GraphicsUtil::transformPosition(Vector2(vertices[i]));
        const Vector vertex2 = GraphicsUtil::transformPosition(Vector2(vertices[i + 1]));
        DrawTriangle(centroid2, {vertex1.x, vertex1.y}, {vertex2.x, vertex2.y}, GRAY);
    }
}

void PolygonRenderer::renderPolygonOutline(const DynamicPolygon &polygon) const {
    std::vector<Vector> vertices = polygon.getWorldSpaceVertices();
    const Vector centroid = GraphicsUtil::transformPosition(Vector2(polygon.getPosition()));

    DrawCircle(static_cast<int>(centroid.x), static_cast<int>(centroid.y), 5.0, RED);

    for (size_t i = 0; i < vertices.size(); ++i) {
        const Vector2 point1 = Vector2(GraphicsUtil::transformPosition(Vector2(vertices[i])));
        const Vector2 point2 = Vector2(GraphicsUtil::transformPosition(Vector2(vertices[(i + 1) % vertices.size()])));
        DrawLineEx({point1.x, point1.y}, {point2.x, point2.y}, 3, RED);
        DrawLine(static_cast<int>(point1.x), static_cast<int>(point1.y), static_cast<int>(centroid.x),
                 static_cast<int>(centroid.y), RED);
    }
}

void PolygonRenderer::renderTexturedPolygon(const std::shared_ptr<DynamicPolygon> &polygon) const {
    const Texture2D texture = resourceManager.getTexture(polygon->getRenderInformation().texture);
    std::vector<Vector> vertices = polygon->getWorldSpaceVertices();
    for (Vector &vertex : vertices) {
        vertex = GraphicsUtil::transformPosition(Vector2(vertex));
    }

    const std::vector<Vector2> raylibVertices = getRaylibVertices(vertices);

    // render outline
    for (size_t i = 0; i < raylibVertices.size(); ++i) {
        const Vector2 &point1 = raylibVertices[i];
        const Vector2 &point2 = raylibVertices[(i + 1) % raylibVertices.size()];
        DrawLineEx({point1.x, point1.y}, {point2.x, point2.y}, 4, BLACK);
    }

    const std::vector<Vector2> &texCoords = polygon->getTextureCoordinates();

    const Vector centroid = GraphicsUtil::transformPosition(Vector2(polygon->getPosition()));
    const Vector2 rlCentroid = {centroid.x, centroid.y};

    // render textured polygon
    this->drawTexturePoly(texture, rlCentroid, raylibVertices, texCoords, static_cast<int>(raylibVertices.size()),
                          WHITE);
}

void PolygonRenderer::drawTexturePoly(const Texture2D &texture,
                                      const Vector2 centroid,
                                      const std::vector<Vector2> &points,
                                      const std::vector<Vector2> &texCoords,
                                      const int pointCount,
                                      const Color tint) const {
    rlSetTexture(texture.id);

    rlBegin(RL_QUADS);

    rlColor4ub(tint.r, tint.g, tint.b, tint.a);

    for (int i = 0; i < pointCount - 1; i++) {
        rlTexCoord2f(0.5f, 0.5f);
        rlVertex2f(centroid.x, centroid.y);

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

std::vector<Vector2> PolygonRenderer::getRaylibVertices(const std::vector<Vector> &vertices) {
    std::vector<Vector2> raylibVertices;
    raylibVertices.reserve(vertices.size() + 1);
    for (const auto &vertex : vertices) {
        raylibVertices.push_back({vertex.x, vertex.y});
    }
    raylibVertices.push_back({vertices[0].x, vertices[0].y});
    return raylibVertices;
}
