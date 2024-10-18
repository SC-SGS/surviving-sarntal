//
// Created by bjoern on 9/20/24.
//

#ifndef POLYGON_H
#define POLYGON_H

#include "../utilities/vector.h"
#include <vector>

struct PolygonEdge {
    Vector vertexA;
    Vector vertexB;
};

class Polygon {
  public:
    /**
     *
     * @return the list of world space coordinates of the polygon's vertices in counterclockwise order
     */
    virtual const std::vector<Vector> &getWorldSpaceVertices() const = 0;
    /**
     *
     * @param point
     * @return whether the point is inside the polygon
     *
    virtual bool isPointInside(const Vector &point) const;*/
    virtual ~Polygon() = default;
};

#endif // POLYGON_H