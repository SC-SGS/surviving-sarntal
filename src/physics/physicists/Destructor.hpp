//
// Created by felix-roehr on 5/27/24.
//

#ifndef SURVIVING_SARNTAL_DESTRUCTOR_HPP
#define SURVIVING_SARNTAL_DESTRUCTOR_HPP

#include "../../entities/World.h"

class Destructor {

  public:
    explicit Destructor(World &world);

    void destruct() const;

  private:
    World &world;

    /**
     * Destructs all rocks that are outside of the world borders.
     */
    void destructRocks() const;

    /**
     * Destruct chunks of the mountain that are outside of the world borders;
     */
    void destructMountain() const;
};

#endif // SURVIVING_SARNTAL_DESTRUCTOR_HPP
