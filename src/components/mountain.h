#pragma once

#include <array>
#include <cmath>
#include <vector>
#include "particle_state.h"



struct IndexInterval {
    std::size_t start_index;
    std::size_t end_index;
};

class Mountain {
  public:
    /**
 * Number of Vertices explicitely stored by the mountain data structure
     */
    static constexpr std::size_t NUMBER_OF_VERTICES{200};

    /**
 * width covered by mountain generated at one point in time
     */
    static constexpr float MOUNTAIN_WIDTH{100.};

    /**
    * distance between two points of mountain
     */
    static constexpr float SECTION_WIDTH{MOUNTAIN_WIDTH/NUMBER_OF_VERTICES};

    static constexpr std::size_t NUM_SECTIONS_PER_CHUNK = 10;

    static constexpr float CHUNK_WIDTH{NUM_SECTIONS_PER_CHUNK*SECTION_WIDTH};

    /**
    * steepness of ramp generated in prototype
     */
    static constexpr float SLOPE{0.5};

    Mountain();

    /**
     * Generates a new Chunk and deletes oldest chunk. Updates internal data
     * structures accordingly. This (prototype) version is only extending the
     * slope
     */
    void generateNewChunk();

    /**
    * Temporary helper function, do not touch
    */
    void printTempDebugInfo();

    /**
     * Example: Your rock x-coords go from [3.1, 4.2]. You call this function
     * which returns [21,24]. This means that the relevant indices for you are
     * 21, 22 and 23 (You DON'T need 24.)
     * @param min_x The leftmost x-coord relevant to you
     * @param max_x The rightmost x-coord relevant to you
     * @return Returns start_index and end_index of the section from min_x to
     * max_x INCLUDING start_index and EXCLUDING end_index;
     */
    IndexInterval getRelevantMountainSection(float min_x, float max_x);

    /** Returns a position from a given index. The index should previously be
     * obtained via a seperate function of the mountain.
     * @param index
     * @return Position (consisting of x- and y-coordinate)
     */
    Position getVertice(size_t index);

    /**
     * @return Returns start_index and end_index of the latest generated
     * chunk. The new chunk INCLUDES start_index and EXCLUDES the end_index. You
     * can access the points via the getVertice-function.
     */
    IndexInterval getLatestChunk();

  private:
    std::array<Position, NUMBER_OF_VERTICES>
        landscape_fixpoints_circular_array{};
    std::size_t start_of_circular_array{0};
};