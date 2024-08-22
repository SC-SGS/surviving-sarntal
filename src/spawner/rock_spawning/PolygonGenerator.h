//
// Created by Anietta Weckauff on 06.08.24.
//

#ifndef SURVIVING_SARNTAL_POLYGONGENERATOR_H
#define SURVIVING_SARNTAL_POLYGONGENERATOR_H

#include "../../utilities/RandomGenerator.hpp"
#include "Polygon.h"

class PolygonGenerator {

  public:
    PolygonGenerator();
    /**
     * This method generates a random convex polygon by first creating a random set of points
     * in a circle (maxRadius, center) and then determining the convex hull with Andrew's monotone chain algorithm.
     * @param numberPoints
     * @param maxRadius
     * @param center
     * @return random convex polygon
     */
    Polygon generatePolygon(int numberPoints, floatType maxRadius, Vector &center);

  private:
    RandomGenerator *randomGenerator = &RandomGenerator::getInstance();
    /**
     * This method generates a random set of numberPoints points in a circle with radius maxRadius
     * around the point center.
     * @param numberPoints
     * @param maxRadius
     * @param center
     * @return set of points
     */
    std::vector<Vector> generatePoints(int numberPoints, floatType maxRadius, Vector &center);

    /**
     * Determines the convex hull of a set of 2D points based on
     * Andrew's monotone chain convex hull algorithm.
     * @param points
     * @return vertices of the convex hull in counter clockwise order
     */
    std::vector<Vector> determineVertices(std::vector<Vector> &points);

    /**
     * Determines the lower hull fo the monotone chain algorithm.
     * @param points
     * @return lower hull
     */
    std::vector<Vector> determineLowerHull(std::vector<Vector> &points);

    /**
     * Determines the upper hull fo the monotone chain algorithm.
     * @param points
     * @return upper hull
     */
    std::vector<Vector> determineUpperHull(std::vector<Vector> &points);

    /**
     * This method calculates the cross product of two vectors OA and OB.
     * If the result is positive, it means that the rotation from 0A to 0B is counter clockwise.
     * If the result is negative, it means that the rotation from 0A to 0B is clockwise.
     * @param vector0
     * @param vectorA
     * @param vectorB
     * @return crossproduct of vector 0A and 0B
     */
    floatType calculateCrossProduct(const Vector &vector0, const Vector &vectorA, const Vector &vectorB);

    /**
     * This method takes a list of vectors in clockwise order and returns them in anticlockwise order.
     * @param vertices
     * @return vertices in anticlockwise order
     */
    std::vector<Vector> reverseOrderToAnticklockWise(std::vector<Vector> &vertices);
};

#endif // SURVIVING_SARNTAL_POLYGONGENERATOR_H
