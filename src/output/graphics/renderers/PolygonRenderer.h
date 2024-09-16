//
// Created by Anietta Weckauff on 09.08.24.
//

#ifndef SURVIVING_SARNTAL_POLYGONRENDERER_H
#define SURVIVING_SARNTAL_POLYGONRENDERER_H

#include "../../../entities/DynamicPolygon.h"
#include "../../ResourceManager.h"

/**
 * This class is responsible for rendering polygons that are defined by a list of vertices.
 */
class PolygonRenderer {

  public:
    explicit PolygonRenderer(ResourceManager &resourceManager);

    /**
     * This method renders the polygon as a filled area.
     * @param polygon
     */
    void renderPolygon(const DynamicPolygon &polygon) const;

    /**
     * This method renders the outline of a polygon based on the vertices the polygon is defined by.
     * @param polygon
     */
    void renderPolygonOutline(const DynamicPolygon &polygon) const;

    /**
     * This method renders a polygon by filling it with a texture.
     * @param polygon
     */
    void renderTexturedPolygon(const DynamicPolygon &polygon) const;

    /**
     *  Draws a textured polygon, defined by vertex and texture coordinates.
     *  Important: polygon center must have a straight line path to all points,
     *  the vertices must be ordered in anticlockwise order,
     *  the texCoords should be in a range from [0, 1]
     *
     * @param texture
     * @param centroid - center of the polygon
     * @param points - vertices of the polygon
     * @param texCoords - coordinates used to map the texture to the polygon
     * @param pointCount - number of vertices of the polygon
     * @param tint
     */
    void drawTexturePoly(const Texture2D &texture, Vector2 centroid, const std::vector<Vector2> &points,
                         const std::vector<Vector2> &texCoords, int pointCount, Color tint) const;

    /**
     * This method transforms the vertices of the polygon into raylib vertices (using Vector2 instead of
     * our data structure vector).
     * Furthermore the first vertice is added to the end of the list, because this is needed for rendering.
     *
     * @return raylib vertices
     */
    static std::vector<Vector2> getRaylibVertices(const std::vector<Vector> &vertices);

  private:
    ResourceManager &resourceManager;
};

#endif // SURVIVING_SARNTAL_POLYGONRENDERER_H
