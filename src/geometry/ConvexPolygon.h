//
// Created by bjoern on 9/19/24.
//

#ifndef CONVEXPOLYGON_H
#define CONVEXPOLYGON_H

#include "Polygon.h"

class ConvexPolygon : public virtual Polygon {
  public:
    std::vector<Vector> getWorldSpaceVertices() const override = 0;
};

#endif // CONVEXPOLYGON_H
