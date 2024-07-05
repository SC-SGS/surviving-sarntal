//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "../utilities/Singleton.hpp"
#include "../utilities/vector.h"
#include <array>
#include <cmath>
#include <memory>
#include <random>
#include <vector>

struct IndexIntervalNew {
    std::size_t startIndex;
    std::size_t endIndex;
};

// TODO this whole class needs to be reworked, which we do anyways later, and a lot of refactoring is necessary
class Mountain {

  public:
    Mountain();
    ~Mountain() = default;
    /**
     * Number of Vertices explicitly stored by the mountain data structure.
     */
    static constexpr std::size_t NUMBER_OF_VERTICES{2048};

    /**
     * Width covered by mountain generated at one point in time.
     */
    static constexpr floatType MOUNTAIN_WIDTH{3 * 2048.};

    /**
     * Distance between two points of mountain.
     */
    static constexpr floatType SECTION_WIDTH{MOUNTAIN_WIDTH / NUMBER_OF_VERTICES};

    static constexpr std::size_t NUM_SECTIONS_PER_CHUNK = 128;

    static constexpr floatType CHUNK_WIDTH{NUM_SECTIONS_PER_CHUNK * SECTION_WIDTH};

    /**
     * steepness of ramp generated in prototype
     */
    static constexpr floatType SLOPE{-0.25}; // TODO fix pls slope should be positive when the mountain goes up

    /**
     * value beween 0 and 1 (prefereably between 0.5 and 0.75)
     */
    static constexpr floatType ROUGHNESS_TERRAIN{0.4};

    /**
     * Generates a new Chunk and deletes oldest chunk. Updates internal data
     * structures accordingly. This (prototype) version is only extending the
     * slope
     */
    void generateNewChunk();

    /**
     * Example: Your rock x-coords go from [3.1, 4.2]. You call this function
     * which returns [21,24]. This means that the relevant indices for you are
     * 21, 22 and 23 (You DON'T need 24.)
     * @param minX The leftmost x-coord relevant to you
     * @param maxX The rightmost x-coord relevant to you
     * @return Returns start_index and end_index of the section from minX to
     * maxX INCLUDING start_index and EXCLUDING end_index;
     */
    // TODO Bezeichner und Doku ändern
    static IndexIntervalNew getRelevantMountainSection(floatType minX, floatType maxX);

    /** Returns a position from a given index. The index should previously be
     * obtained via a separate function of the mountain.
     * @param index
     * @return Position (consisting of x- and y-coordinate)
     */
    Vector getVertex(size_t index) const;

    /** Returns a position from a given index. The index should previously be
     * obtained via a separate function of the mountain.
     * @param index
     * @return Position (consisting of x- and y-coordinate)
     */
    Vector getVertex(int index) const;

    /**
     * You can access all the points currently being held in the mountain
     * datastructures via mountain.getVertex(a) with a in
     * [indexInterval.start_index, indexInterval.end_index) example: the
     * indexInterval goes from 2 to 23. Now you need to access all the points
     * with the indices from 2 to 22
     * @return
     */
    IndexIntervalNew getIndexIntervalOfEntireMountain() const;

    /**
     * @return Returns start_index and end_index of the latest generated
     * chunk. The new chunk INCLUDES start_index and EXCLUDES the end_index. You
     * can access the points via the getVertex-function.
     *
     * Start_index starts with the first new generated vertex.
     * If you want to be precise you would have to also connect the last point
     * of the previously loaded chunk and the chunk you just got by
     * getLatestChunk. You can access that point via getVertex(start_index-1).
     * However, the points might be close enough together that this is not
     * necessary. (If this API sucks it can easily be changed)
     */
    IndexIntervalNew getLatestChunk() const;

    /**
     * Returns the Y coordinate of the mountain at the given x coordinate.
     *
     * TODO there was an offset in this function in the original, because the hiker position was its center, now its his
     * TODO feet
     *
     * @param xPos the x-Coordinate
     * @return the y coordinate on top of the mountain
     */
    floatType getYPosFromX(floatType xPos) const;

    /**
     * Performs simple linear interpolation
     *
     * TODO depending on the mountain, this should become the evaluation of a spline function etc.
     * TODO this function is used as a linInterpolator in one place so it shoudl probably go into a util class.
     *
     * @param xPos
     * @param left
     * @param right
     * @return
     */
    static floatType linearInterpolation(floatType xPos, Vector left, Vector right);

  private:
    std::array<Vector, NUMBER_OF_VERTICES> landscapeFixpointCircularArray{};
    std::size_t startOfCircularArray{0};
    // TODO why are these denominators so weird? they do not adhere to our guidelines!

    /** Generating a mountain using 2D Fractal Terrain Generation as described
     * in this blogpost:
     * http://nick-aschenbach.github.io/blog/2014/07/06/2d-fractal-terrain/
     *
     * @param leftIndex startIndex of the mountain section to be roughened
     * @param rightIndex endIndex of the mountain section to be roughened. The
     * Interval INCLUDES the right index
     * @param displacement Constant defining how aggressive it should be
     * roughened
     */
    void generateTerrainRecursive(std::size_t leftIndex, std::size_t rightIndex, floatType displacement);

    void generateSlope();

    // TODO Does this function really interpolate? What does it do, also ... it is unused
    // void interpolate(std::size_t leftIndex, std::size_t rightIndex);

    static floatType computeDisplacementChange(floatType displacement);

    void updateMidpoint(std::size_t leftIndex, std::size_t rightIndex, std::size_t midIndex, floatType change);

    /**
     * Temporary helper function, do not touch
     * TODO I Don't know what this does and whether we need it
     */
    void printTempDebugInfo() const;
};
