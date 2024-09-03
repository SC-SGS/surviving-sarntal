//
// Created by six on 7/19/24.
//

#ifndef SURVIVING_SARNTAL_MOUNTAINRENDERER_H
#define SURVIVING_SARNTAL_MOUNTAINRENDERER_H

#include "../../../entities/World.h"
#include "../GraphicsUtil.h"
#include "raylib.h"
#include "rlgl.h"

class MountainRenderer {
  public:
    explicit MountainRenderer(Camera2D &camera, GameConstants gameConstants);

    /**
     * Renders the mountain with the given colors in a gradient from top to bottom.
     * The gradient depth is defined by the visual constants.
     * Going deeper into the mountain the color will be constantly the bottom color.
     * @param terr
     * @param topColor is the color at the top of the mountain
     * @param bottomColor is the color at the bottom of the mountain
     */
    void renderMountain(const Terrain &terr, Color topColor = WHITE, Color bottomColor = BLUE);

  private:
    Camera2D &camera;
    GameConstants gameConstants;

    void updateVertices(const Terrain &terrain, Color topColor, Color bottomColor);
    floatType calculateLowerBorder() const;
    void updateVerticesAndColors(const Terrain &terrain, Color topColor, Color bottomColor, int startX);
    void removeOutOfBoundsVerticesAndColors(int newMinX);
    void updateBorders(int newMinX, int newMaxX);
    void addNewVerticesAndColors(const Terrain &terrain, Color topColor, Color bottomColor);

    /**
     * Creates the triangles for the mountain mesh.
     * The triangles are created by connecting the vertices in the correct order.
     * The triangles are created in a zig-zag pattern to create a smooth mountain mesh.
     * The triangles are created in a counter-clockwise order.
     * These triangles then look like this:
     * 0 *-----* 2
     *   |    /|
     *   |   / |
     *   |  /  |
     *   | /   |
     *   |/    |
     * 1 *-----* 3
     *
     * Therefore the indices are: 0, 1, 2 for the first triangle and 1, 3, 2 for the second triangle.
     */
    void createTriangles();
    std::vector<Vector2> vertices;
    std::vector<Vector3> colors;
    std::vector<int> indices;
    int minX = 0;
    int maxX = 0;

    Vector3 normalizeColor(const Color &color) const;
    void addVertexAndColor(const Terrain &terrain, Color topColor, Color bottomColor, int xPos);
    void updateIndices(int indexToRemove);

    /**
     * Draws a mesh of the base of the mountain in a single color.
     * @param index is the index of the lower vertex the 2 triangles should be attached to.
     * @param normalizedColor is the color of the base triangle
     * The mesh then looks like this:
  v[i-1] *-----* v[i+1]
     *   |    /|
     *   |   / |
     *   |  /  |    This is the top mesh that is not affected
     *   | /   |
     *   |/    |
    v[i] *-----* v[i+2]
     *   |    /|
     *   |   / |
     *   |  /  |    This is the bottom mesh that is generated
     *   | /   |
     *   |/    |
     *   *-----*
     * This is the lower border
     */
    void drawBaseTriangle(int index, Vector3 normalizedColor) const;
    void drawMountainBase(Color bottomColor) const;
    void drawMountainMesh() const;
};

#endif // SURVIVING_SARNTAL_MOUNTAINRENDERER_H
