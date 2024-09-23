//
// Created by Anietta Weckauff on 06.08.24.
//

#ifndef SURVIVING_SARNTAL_POLYGONGENERATOR_H
#define SURVIVING_SARNTAL_POLYGONGENERATOR_H

#include "../entities/DynamicPolygon.h"
#include "../utilities/RandomGenerator.hpp"

class PolygonGenerator {
    friend class DynamicPolygon;

  public:
    PolygonGenerator();
    /**
     * This method generates a random convex polygon by first creating a random set of points
     * in a circle (maxRadius, center) and then determining the convex hull with Andrew's monotone chain algorithm.
     * @param numberPoints
     * @param maxRadius
     * @param position
     * @param density
     * @return random convex polygon
     */
    DynamicPolygon
    generatePolygon(int numberPoints, floatType maxRadius, const Vector &position, floatType density) const;

  protected:
    RandomGenerator *randomGenerator = &RandomGenerator::getInstance();
    /**
     * This method generates a random set of numberPoints points in a circle with radius maxRadius
     * around the point center.
     * @param numberPoints
     * @param maxRadius
     * @param center
     * @return set of points
     */
    std::vector<Vector> generatePoints(int numberPoints, floatType maxRadius) const;

    /**
     * Determines the convex hull of a set of 2D points based on
     * Andrew's monotone chain convex hull algorithm.
     * @param points
     * @return vertices of the convex hull in counter clockwise order
     */
    static std::vector<Vector> determineVertices(std::vector<Vector> &points);

    /**
     * Determines the lower hull fo the monotone chain algorithm.
     * @param points
     * @return lower hull
     */
    static std::vector<Vector> determineLowerHull(const std::vector<Vector> &points);

    /**
     * Determines the upper hull fo the monotone chain algorithm.
     * @param points
     * @return upper hull
     */
    static std::vector<Vector> determineUpperHull(const std::vector<Vector> &points);

    /**
     * This method calculates the cross product of two vectors OA and OB.
     * If the result is positive, it means that the rotation from 0A to 0B is counter clockwise.
     * If the result is negative, it means that the rotation from 0A to 0B is clockwise.
     * @param vector0
     * @param vectorA
     * @param vectorB
     * @return crossproduct of vector 0A and 0B
     */
    static floatType calculateCrossProduct(const Vector &vector0, const Vector &vectorA, const Vector &vectorB);

    /**
     * This method calculates the centroid of a polygon.
     * @return centroid
     */
    static Vector calculateCentroid(const std::vector<Vector> &vertices);

    /**
     * This method transforms the vertices of the polygon into coordinates in a new
     * coordinate system which origin is the centroid of the original polygon.
     * @return
     */
    static std::vector<Vector> transformIntoCentroidCoordinateSystem(const Vector &centroid,
                                                                     const std::vector<Vector> &vertices);

    /**
     * This method maps the initial coordinates of the polygon on the texture coordinates
     * which are needed for rendering.
     * The vertices have to be given in anticlockwise order, otherwise the behaviour is undefined.
     * The texture x and y coordinate of the texture are in the interval [0, 1].
     *
     * @return texture coordinates
     */
    static std::vector<Vector2> determineTextureCoordinates(const std::vector<Vector> &vertices);

    /**
     * Calculates the moment of Inertia for the 2D triangle (O, A, B) rotating around the origin O with the z-asxis as
     * rotation axis given its vertices A and B.
     * OAB have to be given in anticlockwise order otherwise the program's behaviour is undefined.
     * The triangle is split into two (signed) right triangles for which the moment of inertia is easy to calculate.
     * We calculate h by letting fall a perpendicular from O on AB.
     * Then we add the signed moments of inertia.
     *
     *          x O                               x O
     *         /|   \                            //|
     *        / | h     \                      / / | h
     *       /  |            \               /  /  |
     *   A  x---x--------------x B      A  x---x B x H
     *       w1 H     w2                   |-- w1 -|
     *                                         |w2-|
     *
     * Signed means that in the left case we add up the inertia of the two right triangles to get the moment of inertia
     * for the big triangle whereas in the right case, we have to subtract the inertia of OBH from OAH to get the total
     * inertia.
     * In total, we use the formula:
     *
     * I(OAB) = I(OAH) + sig * I(OHB)
     *        = density * (h*w1^3/4 + h^3*w1/12 + sig * (h*w2^3/4 + h^3*w2/12))
     *
     * @param vertexA
     * @param vertexB
     * @param density
     * @return
     */
    static floatType
    calculateInertiaOfTriangleRotatingAroundOriginP0(const Vector &vertexA, const Vector &vertexB, floatType density);
    /**
     * This method takes a list of vectors in clockwise order and returns a copy of them in anticlockwise order.
     * @param vertices
     * @return vertices in anticlockwise order
     */
    static std::vector<Vector> reverseOrderToAnticlockWise(std::vector<Vector> vertices);

    /**
     * Calculates the area of a convex polygon given its vertices in a centroid coordinate system.
     * The vertices have to be given in anticlockwise order, otherwise the bahaviour is undefined.
     *
     * @param vertices the vertices in a centroid coordinate system
     * @return the area of the convex polygon
     */
    static floatType calculateAreaBodySpace(const std::vector<Vector> &vertices);

    /**
     * Calculates the moment of inertia of a 2D convex polygon rotating around its centroid with the z-axis as rotation
     * axis given its vertices in a centroid coodinate system. The polygon is split into triangles, where each contains
     * one edge of the polygon and has the centroid (origin) as one of its vertices. The moments of inertia are
     * calculated for each triangle rotating around the centroid and then summed up.
     * Make sure, the vertices are actually given in a centroid coordinate system and in an anticlockwise order.
     *
     * @param vertices the vertices in a centroid coordinate system
     * @param density the constant density of the polygon (a homogenous material is assumed, not a density function)
     * @return the moment of inertia of the convex polygon
     */
    static floatType calculateInertiaRotCentroidBodySpace(const std::vector<Vector> &vertices, floatType density);

    /**
     * Checks whether a given list of vertices is given in anticlockwise ordering.
     * It does not check the total ordering, but only that each two adjacent vertices are given in anticlockwise order.
     *
     * @param vertices
     * @return
     */
    static bool isAnticlockwise(const std::vector<Vector> &vertices);
};

#endif // SURVIVING_SARNTAL_POLYGONGENERATOR_H
