//
// Created by felix-roehr on 7/29/24.
//

#ifndef SURVIVING_SARNTAL_MOUNTAIN_HPP
#define SURVIVING_SARNTAL_MOUNTAIN_HPP

#include "Biome.hpp"
class Mountain : public Biome {
  public:
    Mountain(Vector basePoint, Vector baseDerivative, floatType startT, floatType length, HikerConstants hikerConstants,
             TerrainConstants terrainConstants);
    ~Mountain() override = default;

  private:
    const static std::vector<TerrainPhase> mountainPhaseChoices;
};

#endif // SURVIVING_SARNTAL_MOUNTAIN_HPP
