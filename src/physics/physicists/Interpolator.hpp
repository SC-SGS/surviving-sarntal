//
// Created by bjoern on 5/21/24.
//

#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#include "../../entities/World.h"
#include "../../utilities/Singleton.hpp"

class Interpolator {

  public:
    explicit Interpolator(World &world);
    ~Interpolator() = default;
    /**
     * @brief Interpolates the positions of all entities.
     *
     * The idea is to interpolate the positions between the last and current position based on the time left in the
     * accumulator after a physics step. May increase visual accuracy but is currently not implemented and deemed
     * unnecessary.
     *
     * @param alpha blending factor of current and previous state (the two last states before the next frame)
     */
    void interpolate(floatType alpha) const;

  private:
    const World &world;
};

#endif // INTERPOLATOR_H
