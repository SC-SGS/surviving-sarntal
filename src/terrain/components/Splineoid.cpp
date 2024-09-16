//
// Created by felix-roehr on 7/25/24.
//

#include "Splineoid.hpp"

#include <utility>
Splineoid::Splineoid(std::shared_ptr<StaticPolygon> polygon, const std::vector<Vector> &derivatives,
                     TerrainConstants &terrainConstants)
    : ClosedComponent(std::move(polygon), terrainConstants) {}

Vector Splineoid::evaluate(floatType relativeT) const { return ClosedComponent::evaluate(relativeT); }
