//
// Created by felix-roehr on 7/25/24.
//

#include "Splineoid.hpp"
Splineoid::Splineoid(StaticPolygon *polygon, const std::vector<Vector> &derivatives, TerrainConstants terrainConstants)
    : ClosedComponent(polygon, terrainConstants) {}

Vector Splineoid::evaluate(floatType relativeT) const { return ClosedComponent::evaluate(relativeT); }
