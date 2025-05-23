//
// Created by felix-roehr on 7/25/24.
//

#include "ClosedComponent.hpp"

#include <utility>

ClosedComponent::ClosedComponent(std::shared_ptr<StaticPolygon> polygon, TerrainConstants &terrainConstants)
    : Component(std::move(polygon), terrainConstants) {}

floatType ClosedComponent::getLeftBorder() { return this->polyObject->getBoundingBox().minMin.x; }
floatType ClosedComponent::getRightBorder() { return this->polyObject->getBoundingBox().maxMax.x; }
floatType ClosedComponent::getUpperBorder() { return this->polyObject->getBoundingBox().maxMax.y; }
floatType ClosedComponent::getLowerBorder() { return this->polyObject->getBoundingBox().minMin.y; }

Vector ClosedComponent::evaluate(floatType relativeT) const { return {}; }
