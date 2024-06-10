//
// Created by bjoern on 5/21/24.
//

#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#include "../../entities/World.h"
#include "../../utilities/Singleton.hpp"

class Interpolator : public Singleton<Interpolator> {
    friend class Singleton<Interpolator>;

  public:
    /**
     * Probably linear interpolation for positions and Slerp for orientations.
     * @param alpha blending factor of current and previous state (the two last states before the next frame)
     */
    void interpolate(floatType alpha) const;

  private:
    const World &world;
    Interpolator();
    ~Interpolator();
};

#endif // INTERPOLATOR_H
