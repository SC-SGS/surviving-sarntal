//
// Created by six on 7/19/24.
//

#ifndef SURVIVING_SARNTAL_MOUNTAINRENDERER_H
#define SURVIVING_SARNTAL_MOUNTAINRENDERER_H

#include "../../../entities/World.h"
#include "../GraphicsUtil.h"
#include "poly2tri/common/shapes.h"
#include "raylib.h"
#include "rlgl.h"

class MountainRenderer {
  public:
    explicit MountainRenderer(Camera2D &camera, GameConstants &gameConstants, ResourceManager &resourceManager);

    /**
     * Renders the mountain with the given colors in a gradient from top to bottom.
     * The gradient depth is defined by the visual constants.
     * Going deeper into the mountain the color will be constantly the bottom color.
     * @param terr
     * @param topColor is the color at the top of the mountain
     * @param bottomColor is the color at the bottom of the mountain
     */
    void renderMountain(const Terrain &terrain, bool debug);
    void debugRenderMountain();

    void reset();

  private:
    Camera2D &camera;
    GameConstants &gameConstants;

    void updateVertices(const Terrain &terrain);

    std::vector<Vector2> vertices;
    std::vector<Vector3> colors;
    std::vector<int> indices;
    floatType minX = 0;
    floatType maxX = 0;
    void drawMountainMesh() const;

    void drawSurface(const Terrain &terrain) const;

    Texture2D mountainTexture{};
    bool verticesNeedUpdate = true; // Flag to indicate if triangulation is needed
    std::vector<p2t::Point *> createPolylinePoints(const std::shared_ptr<StaticPolyline> &ground) const;
};

#endif // SURVIVING_SARNTAL_MOUNTAINRENDERER_H
