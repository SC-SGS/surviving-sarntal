#pragma once

#include <array>
#include <cmath>
#include <vector>
#include "../components/particle_state.h"

const float MOUNTAIN_WIDTH{
    100.}; // width covered by mountain generated at one point in time
const float SECTION_WIDTH{0.5};
const std::size_t NUMBER_OF_FIXPOINTS{
    200}; // number_of_fixpoints = (size_t) std::ceil(mountain_width /
          // section_width), manually adjust that if needed
const float CHUNK_WIDTH{5.};

const float SLOPE{0.5}; // steepness of ramp generated in prototype


struct IndexInterval {
    std::size_t start_indice;
    std::size_t end_indice;
};

class Mountain {
  private:
    std::array<Position, NUMBER_OF_FIXPOINTS>
        landscape_fixpoints_circular_array{};
    std::size_t start_of_circular_array{0};

  public:
    Mountain();

    /**
     * Generates a new Chunk and deletes oldest chunk. Updates internal data
     * structures accordingly. This (prototype) version is only extending the
     * slope
     */
    void generateNewChunk();

    void printTempDebugInfo();

    /**
     * Example: Your rock x-coords go from [3.1, 4.2]. You call this function
     * which returns [21,24]. This means that the relevant indices for you are
     * 21, 22 and 23 (You DON'T need 24.)
     * @param min_x The leftmost x-coord relevant to you
     * @param max_x The rightmost x-coord relevant to you
     * @return Returns start_indice and end_indice of the section from min_x to
     * max_x INCLUDING start_indice and EXCLUDING end_indice;
     */
    IndexInterval getRelevantMountainSection(float min_x, float max_x);

    /** Returns a position from a given index. The index should previously be
     * obtained via a seperate function of the mountain.
     * @param index
     * @return Position (consisting of x- and y-coordinate)
     */
    Position getFixpoint(size_t index);

    /**
     * @return Returns start_indice and end_indice of the latest generated
     * chunk. The new chunk INCLUDES start_index and EXCLUDES the end_index. You
     * can access the points via the getFixpoint-function.
     */
    IndexInterval getLatestChunk();
};