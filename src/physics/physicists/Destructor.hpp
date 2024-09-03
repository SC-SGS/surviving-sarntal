//
// Created by felix-roehr on 5/27/24.
//

#ifndef SURVIVING_SARNTAL_DESTRUCTOR_HPP
#define SURVIVING_SARNTAL_DESTRUCTOR_HPP

#include "../../entities/World.h"
#include "../../output/graphics/Renderer.h"
#include "../../utilities/Singleton.hpp"

class Destructor {

  public:
    void destruct() const;
    explicit Destructor(World &world, Renderer &renderer, GameConstants gameConstants);
    ~Destructor() = default;

  private:
    World &world;
    Renderer &renderer;
    GameConstants gameConstants;

    /**
     * Destructs all rocks that are outside of the world borders.
     */
    void destructRocks() const;

    /**
     * Destructs all items that are outside of the world borders.
     */
    void destructItems() const;

    /**
     * Destruct biomes of the terrain that are outside of the world borders;
     */
    void destructTerrain() const;

    /**
     * Kill hiker if necessary.
     */
    void destructHiker() const;
};

#endif // SURVIVING_SARNTAL_DESTRUCTOR_HPP
