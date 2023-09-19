#pragma once

#include <vector>
#include <array>
#include <cmath>

const float mountain_width{100.}; //width covered by mountain generated at one point in time
const float section_width{0.5};
const std::size_t number_of_fixpoints{200}; //number_of_fixpoints = (size_t) std::ceil(mountain_width / section_width), manually adjust that if needed
const float chunk_width{5.};

const float slope{0.5};   //steepness of ramp generated in prototype

struct Position{
    float x;
    float y;
};

struct IndexInterval{
    std::size_t start_indice;
    std::size_t end_indice;
};

class Mountain{
private:
    std::array<Position, number_of_fixpoints> landscape_fixpoints_circular_array{};
    std::size_t start_of_circular_array{0};
public:
    Mountain();

    void generateNewChunk();

    void printTempDebugInfo();

    IndexInterval getRelevantMountainSection(float min_x, float max_x);

    Position getFixpoint(size_t index);
};