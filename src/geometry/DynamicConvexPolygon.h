//
// Created by Anietta Weckauff on 06.08.24.
//

#ifndef DYNAMICCONVEXPOLYGON_H
#define DYNAMICCONVEXPOLYGON_H

#include "../entities/RenderedEntity.h"
#include "../utilities/vector.h"
#include "AABB.h"
#include "ConvexPolygon.h"

#include <map>
#include <vector>

/**
 * Dynamic Attributes: Last Position (current is inherited from Rendered Entity), Rotation (in radians) and linear and
 * angular momentum (for rocks).
 */
struct DynamicProperties {
    Vector lastPosition{};
    floatType rotationAngleRad = 0.f;
    floatType lastRotationAngleRad = 0.f;
    Vector linearMomentum{};
    floatType angularMomentum = 0.f;
};

/**
 * Contains info about the last witness found to another rock.
 * A witness is a separating axis (see SAT).
 * If we fina a separating axis to another polygon p, the map stores the tuple (p.ID, index of edge on this polygon that
 * is a witness). The map is used to boost performance during collision detection with other rocks, because we can
 * immediately return after finding any separating axis. If we do not have a witness to another polygon, usually the
 * witness index is set to 0, i.e. we start the next CD step with the first edge as we have no further knowledge.
 */
struct CollisionData {
    std::map<size_t, size_t> lastWitness{};
};

/**
 * A polygon is represented by a set of points which define the vertices of the object.
 * The postition of the polygon is the position of its centroid and the vertices are stored in body coordinates.
 * When needed, they are transformed to world coordinates by rotation and subsequent translation.
 */
class DynamicConvexPolygon : public RenderedEntity, public virtual ConvexPolygon {

  protected:
    std::vector<Vector> worldSpaceVertices{};
    AABB sweptAABB{{0, 0}, {0, 0}};
    AABB currentAABB{{0, 0}, {0, 0}};
    DynamicProperties dynamicProperties;
    static size_t polyIDCount;
    const size_t polyID;
    CollisionData collisionData;

    // Static Attributes:
    // Body Space Coordinates of the corners in anticlockwise order
    const std::vector<Vector> bodySpaceVertices;
    const std::vector<Vector2> textureCoordinates;
    const floatType mass;
    const floatType density;
    const floatType momentOfInertia;

    /**
     * Always call this method to make sure world space vertices, current and swept bounding box are consistent.
     * world space coords should always be updated first, then the current bounding box and only then the swept bounding
     * box. This order is necessary due to time efficient optimizations.
     */
    void updateAfterMovementOrRotation();

  private:
    void recalculateWorldSpaceVertices();
    void recalculateSweptBoundingBox();
    void recalculateCurrentBoundingBox();

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
     * @param density
     * @param momentOfInertia
     * @param dynamicProperties
     * @throw std::invalid_argument if the conditions above are not met
     */
    DynamicConvexPolygon(const Vector &position,
                         const std::vector<Vector> &vertices,
                         const std::vector<Vector2> &textureCoordinates,
                         floatType mass,
                         floatType density,
                         floatType momentOfInertia,
                         const DynamicProperties &dynamicProperties);

    floatType getRotationAngle() const;
    floatType getMass() const;
    floatType getDensity() const;
    floatType getMomentOfInertia() const;
    size_t getPolyID() const;
    void setPosition(const Vector &position) override;

    Vector getVelocityAtPointInWorldSpace(const Vector &point) const;
    Vector getVelocityAtPointInBodySpace(const Vector &point) const;
    Vector getVelocityAtVertex(size_t vertexIdx) const;

    /**
     * Checks for equality via unique id.
     *
     * @param other
     * @return
     */
    bool operator==(const DynamicConvexPolygon &other) const;

    /**
     * Checks for inequality via unique id.
     *
     * @param other
     * @return
     */
    bool operator!=(const DynamicConvexPolygon &other) const;

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
    const std::vector<Vector> &getWorldSpaceVertices() const override;

    /**
     * Returns the interpolated list of vertices at time last time + alpha * (current time - last time).
     *
     * @param alpha between 0 and 1
     * @return
     */
    std::vector<Vector> interpolateWorldSpaceVerticesBetweenLastAndCurrent(floatType alpha) const;

    /**
     * @return the current bounding box
     */
    const AABB &getBoundingBox() const;

    /**
     * Calculates bounding box at a time alpha between last and current time (alpha in [0, 1]).
     *
     * @param alpha
     * @return
     */
    AABB getInterpolatedBoundingBox(floatType alpha) const;

    /**
     * Returns the swept bounding box of this polygon, calculated from its last and current position.
     *
     * @return
     */
    const AABB &getSweptBoundingBox() const;

    /**
     * This method moves the polygon.
     * The last position is set to the current position and then the offset is added to the current position.
     * @param offset change of position
     */
    void move(const Vector &offset);

    /**
     * This method rotates the polygon by the given angularOffset in radians. Different to setRotationAngle because it
     * also sets last rotation angle to the one before the method call.
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

    void setInterpolatedMovementState(floatType alpha);
    floatType getInterpolatedRotationAngleRad(floatType alpha) const;
    Vector getInterpolatedPosition(floatType alpha) const;

    const Vector &getLinearMomentum() const;
    floatType getAngularMomentum() const;
    floatType getRotationAngleRad() const;
    const DynamicProperties &getDynamicProperties() const;

    void updateLastWitness(size_t otherRockID, size_t witnessEdgeIdx);
    size_t getLastWitnessTo(size_t otherRockID) const;
    void removeWitnessInformationFor(size_t otherID);
    void removeAllWitnessInformation();

    void applyForce(const Vector &force, const Vector &point);
};

#endif //  DYNAMICCONVEXPOLYGON_H