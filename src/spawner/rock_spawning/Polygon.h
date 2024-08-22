//
// Created by Anietta Weckauff on 06.08.24.
//

#ifndef SURVIVING_SARNTAL_POLYGON_H
#define SURVIVING_SARNTAL_POLYGON_H

#include "../../utilities/vector.h"
#include <iostream>
#include <vector>

/**
 * A polygon is represented by a set of points which define the vertices of the object.
 */
class Polygon {

  private:
    std::vector<Vector> vertices;
    std::vector<Vector2> textureCoordinates = determineTextureCoordinates();

  public:
    explicit Polygon(std::vector<Vector> &vertices);

    /**
     * This method returns the list of vertices that define the polygon.
     * @return
     */
    std::vector<Vector> getVertices();

    /**
     * This method creates a bounding box for the polygon in the shape of a rectangle.
     * The x coordinate is the minX and the y coordinate is the minY.
     * @return bounding box
     */
    Rectangle getBoundingBox();

    /**
     * This method calculates the centroid of a polygon.
     * @return centroid
     */
    Vector calculateCentroid();

    /**
     * This method calculates the center of a polygon (by calculating the mean of the x coordinates
     * and the mean of the y coordinates)
     * @return center
     */
    Vector calculateCenter();

    /**
     * This method moves the polygon.
     * @param xOffset - how much it is moved in x direction
     * @param yOffset - how much it is moved in y direction
     */
    void move(floatType xOffset, floatType yOffset);

    /**
     * This method rotates the polygon by the given angularOffset.
     * @param angularOffset
     */
    void rotate(floatType additionalAngularOffset);

    /**
     * This method transforms the vertices of the polygon into raylib vertices (using Vector2 instead of
     * our data structure vector).
     * Furthermore the first vertice is added to the end of the list, because this is needed for rendering.
     *
     * @return raylib vertices
     */
    std::vector<Vector2> getRaylibVertices();

    /**
     * This method maps the initial coordinates of the polygon on the texture coordinates
     * which are needed for rendering.
     * The texture x and y coordinate of the texture are in the interval [0, 1].
     *
     * @return texture coordinates
     */
    std::vector<Vector2> determineTextureCoordinates();

    std::vector<Vector2> getTextureCoordinates();

    /**
     * This method transforms the vertices of the polygon into coordinates in a new
     * coordinate system which origin is the centroid of the original polygon.
     * @return
     */
    std::vector<Vector> transformIntoCentroidCoordinateSystem();
};

#endif // SURVIVING_SARNTAL_POLYGON_H
