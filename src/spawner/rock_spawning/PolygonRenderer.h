//
// Created by Anietta Weckauff on 09.08.24.
//

#ifndef SURVIVING_SARNTAL_POLYGONRENDERER_H
#define SURVIVING_SARNTAL_POLYGONRENDERER_H

#include "../../output/ResourceManager.h"
#include "Polygon.h"

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
    void renderPolygon(Polygon &polygon);

    /**
     * This method renders the outline of a polygon based on the vertices the polygon is defined by.
     * @param polygon
     */
    void renderPolygonOutline(Polygon &polygon);

    /**
     * This method renders a polygon by filling it with a texture.
     * @param polygon
     */
    void renderTexturedPolygon(Polygon &polygon);

    /**
     *  Draws a textured polygon, defined by vertex and texture coordinates.
     *  Important: polygon center must have a straight line path to all points,
     *  the vertices must be ordered in anticlockwise order,
     *  the texCoords should be in a range from [0, 1]
     *
     * @param texture
     * @param center - center of the polygon
     * @param points - vertices of the polygon
     * @param texCoords - coordinates used to map the texture to the polygon
     * @param pointCount - number of vertices of the polygon
     * @param tint
     */
    void drawTexturePoly(Texture2D texture, Vector2 center, Vector2 *points, Vector2 *texCoords, int pointCount,
                         Color tint);

  private:
    ResourceManager &resourceManager;
};

#endif // SURVIVING_SARNTAL_POLYGONRENDERER_H
