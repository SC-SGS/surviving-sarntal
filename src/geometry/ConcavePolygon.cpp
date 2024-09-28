//
// Created by bjoern on 9/20/24.
//

#include "ConcavePolygon.h"

std::vector<PolygonEdge> ConcavePolygon::getEdgesFromWorldSpaceCoords(const std::vector<Vector> &coords) {
    std::vector<PolygonEdge> edges{coords.size()};
    for (size_t i = 0; i < coords.size(); ++i) {
        edges[i] = PolygonEdge{coords[i], coords[(i + 1) % coords.size()]};
    }
    return edges;
}

std::vector<Vector> ConcavePolygon::getWorldSpaceCoordsFromEdges(const std::vector<PolygonEdge> &edges) {
    assert(edgeChainIsContinuousAndClosed(edges));
    std::vector<Vector> coords{edges.size()};
    for (size_t i = 0; i < edges.size(); ++i) {
        coords[i] = edges[i].vertexA;
    }
    return coords;
}
bool ConcavePolygon::edgeChainIsContinuous(const std::vector<PolygonEdge> &edgeChain) {
    for (size_t i = 0; i < edgeChain.size() - 1; ++i) {
        if (edgeChain[i].vertexB != edgeChain[i + 1].vertexA) {
            return false;
        }
    }
    return true;
}

bool ConcavePolygon::edgeChainIsContinuousAndClosed(const std::vector<PolygonEdge> &edgeChain) {
    for (size_t i = 0; i < edgeChain.size(); ++i) {
        if (edgeChain[i].vertexB != edgeChain[(i + 1) % edgeChain.size()].vertexA) {
            return false;
        }
    }
    return true;
}