//
// Created by felix-roehr on 9/7/24.
//

#ifndef SURVIVING_SARNTAL_GENERATIONRENDERER_HPP
#define SURVIVING_SARNTAL_GENERATIONRENDERER_HPP

#include "../../output/ResourceManager.h"
#include "../../utilities/vector.h"
#include "../biomes/TerrainPhase.hpp"
#include "../components/Ground.hpp"
#include "../points/StaticPolyline.hpp"

class GenerationRenderer {
  public:
    GenerationRenderer(HikerConstants &hikerConstants, TerrainConstants &terrainConstants,
                       ResourceManager &resourceManager);
    ~GenerationRenderer() = default;

    const void render(const std::shared_ptr<Ground> &newGround,
                      const std::shared_ptr<StaticPolyline> &groundPolyRepresentation, Vector &newPoint,
                      TerrainPhase &phase, floatType minAngle, floatType maxAngle);

  private:
    TerrainConstants &terrainConstants;
    HikerConstants &hikerConstants;
    ResourceManager &resourceManager;

    Camera2D setUpRendering() const;

    static Vector2 transformPosition(Vector position);

    void renderBackground();
};

#endif // SURVIVING_SARNTAL_GENERATIONRENDERER_HPP
