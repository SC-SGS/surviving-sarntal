//
// Created by bjoern on 5/21/24.
//

#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#include "../../entities/World.h"

class Interpolator {

  public:
    explicit Interpolator(const World &world);

    /**
     * Probably linear interpolation for positions and Slerp for orientations.
     * @param alpha blending factor of current and previous state (the two last states before the next frame)
     */
    void interpolate(float alpha) const;

  private:
    const World &world;
};

#endif // INTERPOLATOR_H
