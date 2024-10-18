//
// Created by bjoern on 9/20/24.
//

#ifndef CONCAVEPOLYGON_H
#define CONCAVEPOLYGON_H
#include "../utilities/vector.h"
#include "Polygon.h"

#include <cassert>

class ConcavePolygon final : public virtual Polygon {
  public:
    explicit ConcavePolygon(const std::vector<Vector> &worldSpaceCoordinates)
        : worldSpaceCoordinates(worldSpaceCoordinates), edges(getEdgesFromWorldSpaceCoords(worldSpaceCoordinates)) {}

    explicit ConcavePolygon(const std::vector<PolygonEdge> &edges)
        : worldSpaceCoordinates(getWorldSpaceCoordsFromEdges(edges)), edges(edges) {
        assert(worldSpaceCoordinates[0] == worldSpaceCoordinates[worldSpaceCoordinates.size()]);
    }
    const std::vector<Vector> &getWorldSpaceVertices() const override { return this->worldSpaceCoordinates; }

  private:
    const std::vector<Vector> worldSpaceCoordinates;
    const std::vector<PolygonEdge> edges;
    static std::vector<PolygonEdge> getEdgesFromWorldSpaceCoords(const std::vector<Vector> &coords);
    static std::vector<Vector> getWorldSpaceCoordsFromEdges(const std::vector<PolygonEdge> &edges);
    static bool edgeChainIsContinuous(const std::vector<PolygonEdge> &edgeChain);
    static bool edgeChainIsContinuousAndClosed(const std::vector<PolygonEdge> &edgeChain);
};

#endif // CONCAVEPOLYGON_H
