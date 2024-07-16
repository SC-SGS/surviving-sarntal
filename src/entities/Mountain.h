//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "../utilities/BezierSpline.hpp"
#include "../utilities/HermiteSpline.hpp"
#include "../utilities/RandomGenerator.hpp"
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
     * Generate a new chunk interpolating a new random point b. For this, we add a new spline piece that respects the
     * previous point a, and its first and second derivatives da, (da)^2.
     */
    void generateNewChunk();

    /**
     * Removes the leftmost chunk.
     */
    void deleteLeftChunk();

    /**
     * Calculates the yPos at the given xPos of the mountain.
     *
     * @param xPos
     * @return yPos
     */
    floatType calculateYPos(floatType xPos) const;

    /**
     * Calculates the derivative of the mountain at the given xPos.
     *
     * @param xPos
     * @return
     */
    floatType calculateDerivative(floatType xPos) const;

    /**
     * Calculates the n-th derivative of the mountain at the given xPos.
     *
     * @param xPos
     * @return
     */
    floatType calculateDerivative(floatType xPos, int n) const;

    /**
     * Calculates the vector corresponding to the derivative of the mountain at the given point.
     *
     * @param xPos
     * @return derivative vector
     */
    Vector calculateDerivativeVector(floatType xPos) const;

    /**
     * Calculates the vector orthogonal to the derivative of the mountain at the given point.
     * Based on multiplication with a rotation matrix:
     * R =  (  0   -1  )
     *      (  1    0  )
     *
     * @param xPos
     * @return normal vector
     */
    Vector calculateNormalVector(floatType xPos) const;

    floatType getLeftBorder() const;

    floatType getRightBorder() const;

    bool isInRange(floatType xPos) const;

  private:
    std::vector<Vector> positions = {};
    std::vector<floatType> derivatives = {};
    std::vector<HermiteSpline *> spline = {};
    RandomGenerator *rand = &RandomGenerator::getInstance();

    /**
     * The initial chunk consists of one hermite spline with natural border conditions (Second evaluateDerivative at
     * left and right border need to be equal).
     */
    void generateInitialChunk();

    /**
     * Generates the entire mountain. Generates enough chunks to fill the entire screen.
     */
    void generateMountain();
};
