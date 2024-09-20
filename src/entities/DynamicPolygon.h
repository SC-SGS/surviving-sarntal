//
// Created by Anietta Weckauff on 06.08.24.
//

#ifndef SURVIVING_SARNTAL_POLYGON_H
#define SURVIVING_SARNTAL_POLYGON_H

#include "../utilities/vector.h"
#include "RenderedEntity.h"

#include <vector>

/**
 * Dynamic Attributes: Last Position (current is inherited from Rendered Entity), Rotation (in radians) and linear and
 * angular momentum (for rocks).
 */
struct DynamicProperties {
    Vector lastPosition{};
    floatType rotationAngleRad = 0;
    Vector linearMomentum{};
    floatType angularMomentum{};
};

/**
 * A polygon is represented by a set of points which define the vertices of the object.
 * The postition of the polygon is the position of its centroid and the vertices are stored in body coordinates.
 * When needed, they are transformed to world coordinates by rotation and subsequent translation.
 */
class DynamicPolygon : public RenderedEntity {

  protected:
    DynamicProperties dynamicProperties;

    // Static Attributes:
    // Body Space Coordinates of the corners in anticlockwise order
    const std::vector<Vector> bodySpaceVertices;
    const std::vector<Vector2> textureCoordinates;
    const floatType mass;
    const floatType density;
    // TODO inertia tensor or moment of inertia in each step
    const floatType momentOfInertia;

    floatType calculateDensity(const std::vector<Vector> &vertices, floatType mass) const;
    floatType calculateMomentOfInertia(const std::vector<Vector> &vertices, floatType mass) const;

  public:
    /**
     * Constructs a Dynamic Polygon from the given parameters. However, to create a polygon that follows the specified
     * behaviour, it is recommended to use the PolygonGenerator.
     * When calling this method directly, the client should be aware of
     * the semantic restrictions:
     * - The vertices should be given as coordinates in a coordinate system with the centroid of the convex polygon as
     *   its origin.
     * - They should be ordered anticlockwise.
     * - There must be exactly n+1 texture coordintes if the polygon has n vertices and
     * - the last texture coordinate is equal to the first since it closes the polygon.
     * - The polygon must be free of self-intersections.
     * If these conditions are not met, the behaviour is unspecified and may result in errors.
     *
     * @param position
     * @param vertices
     * @param textureCoordinates
     * @param mass
     * @param momentOfInertia
     * @param dynamicProperties
     * @throw std::invalid_argument if the conditions above are not met
     */
    DynamicPolygon(const Vector &position, const std::vector<Vector> &vertices,
                   const std::vector<Vector2> &textureCoordinates, floatType mass, floatType density,
                   floatType momentOfInertia, const DynamicProperties &dynamicProperties);

    /**
     * This is a simplified constructor for Dynamic Polygon that is only used
     * to initialize a bounding box in the shape of a convex polygon for the hiker.
     * @param position
     * @param vertices
     * @param mass
     */
    DynamicPolygon(const Vector &position, const std::vector<Vector> &vertices, floatType mass);

    floatType getRotationAngle() const;
    floatType getMass() const;
    floatType getDensity() const;
    floatType getMomentOfInertia() const;

    /**
     * This method returns the list of vertices that define the polygon in body space coordinates i.e. with the centroid
     * as the origin.
     * @return
     */
    const std::vector<Vector> &getBodySpaceVertices() const;

    /**
     * This method returns the list of vertices that define the polygon in world space coordinates.
     * @return
     */
    std::vector<Vector> getWorldSpaceVertices() const;

    /**
     * This method creates a bounding box for the polygon in the shape of a rectangle.
     * The x coordinate is the minX and the y coordinate is the minY.
     * @return bounding box
     */
    Rectangle getBoundingBox() const;

    /**
     * This method moves the polygon.
     * The last position is set to the current position and then the offset is added to the current position.
     * @param xOffset - how much it is moved in x direction
     * @param yOffset - how much it is moved in y direction
     */
    void move(floatType xOffset, floatType yOffset);

    /**
     * This method moves the polygon.
     * The last position is set to the current position and then the offset is added to the current position.
     * @param offset change of position
     */
    void move(const Vector &offset);

    /**
     * This method rotates the polygon by the given angularOffset in radians.
     * @param angularOffset
     */
    void rotate(floatType angularOffset);

    const std::vector<Vector2> &getTextureCoordinates() const;

    /**
     * The polygon has no animation and uses just a default texture.
     * Note: Height and width are set to 0 as rendering is performed by the polygon renderer.
     * The radian rotation angle is transformed to degrees in the render information.
     *
     * @return the render information
     */
    RenderInformation getRenderInformation() const override;

    void setLinearMomentum(const Vector &newLinearMomentum);
    void setAngularMomentum(floatType newAngularMomentum);
    void setRotationAngleRad(floatType newRotationAngleRad);

    const Vector &getLinearMomentum() const;
    floatType getAngularMomentum() const;
    floatType getRotationAngleRad() const;
    const DynamicProperties &getDynamicProperties() const;
};

#endif // SURVIVING_SARNTAL_POLYGON_H
