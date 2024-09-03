//
// Created by felix-roehr on 7/25/24.
//

#ifndef SURVIVING_SARNTAL_CLOSEDCOMPONENT_HPP
#define SURVIVING_SARNTAL_CLOSEDCOMPONENT_HPP

#include "../../game/GameProperties.hpp"
#include "../../utilities/vector.h"
#include "../points/StaticPolygon.hpp"
#include "../spatialDatastructure/AxisAlignedBoundingBox.hpp"
#include "Component.hpp"

class ClosedComponent : public Component<StaticPolygon> {
  public:
    ClosedComponent(StaticPolygon *polygon, TerrainConstants terrainConstants);

    floatType getLeftBorder();

    floatType getRightBorder();

    floatType getUpperBorder();

    floatType getLowerBorder();

    Vector evaluate(floatType relativeT) const override;
};

#endif // SURVIVING_SARNTAL_CLOSEDCOMPONENT_HPP
