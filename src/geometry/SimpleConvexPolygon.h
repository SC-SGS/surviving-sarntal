//
// Created by bjoern on 9/19/24.
//

#ifndef SIMPLECONVEXPOLYGON_H
#define SIMPLECONVEXPOLYGON_H

#include "../utilities/vector.h"
#include "ConvexPolygon.h"

class SimpleConvexPolygon : public ConvexPolygon {
  public:
    explicit SimpleConvexPolygon(const std::vector<Vector> &worldSpaceCoordinates)
        : worldSpaceCoordinates(worldSpaceCoordinates) {}
    const std::vector<Vector> &getWorldSpaceVertices() const override { return this->worldSpaceCoordinates; }

  private:
    std::vector<Vector> worldSpaceCoordinates;
};

#endif // SIMPLECONVEXPOLYGON_H
