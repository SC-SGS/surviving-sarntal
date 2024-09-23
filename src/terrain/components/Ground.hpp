//
// Created by felix-roehr on 7/25/24.
//

#ifndef SURVIVING_SARNTAL_GROUND_HPP
#define SURVIVING_SARNTAL_GROUND_HPP

#include "../points/StaticPolyline.hpp"
#include "../splines/TwoDimensionalHermiteSpline.hpp"
#include "Component.hpp"
class Ground : public Component<StaticPolyline> {
  public:
    Ground(const std::shared_ptr<StaticPolyline> &polyline,
           const std::vector<Vector> &derivatives,
           floatType startT,
           const TerrainConstants &terrainConstants);

    /**
     * Adds a 2D-Spline to the end of the given ground that starts at the currently last point.
     *
     * @param newPoint
     * @param newDerivative
     */
    void addTerrain(Vector &newPoint, Vector &newDerivative);

    /**
     * Adds a 2D-spline representation of the given polyline using the given derivatives at the individual points to
     * connect the splines.
     *
     * @param polyline
     * @param derivatives
     */
    void addTerrain(const std::shared_ptr<StaticPolyline> &polyline, const std::vector<Vector> &derivatives);

    ///**
    // * Merges the given ground to the rights side of this one, if the startPoint of the new ground matches the end
    // point
    // * of this ground and the derivatives at the specified locations match as well.
    // *
    // * @param ground
    // */
    // void addTerrain(const Ground &ground);

    /**
     * Returns the part of the polyline that would be added to the polynomial representation of the ground if the new
     * point was added to the existing terrain.
     *
     * @param newPoint
     * @param newDerivative
     * @param resolution
     * @return
     */
    std::shared_ptr<StaticPolyline>
    getPolyRepresentationLastPointToNewPoint(Vector &newPoint, Vector &newDerivative, floatType resolution) const;

    Vector evaluate(floatType relativeT) const override;

    /**
     * Computes the part of the polyline representation of the ground that is within the generation checking range.
     *
     * @return
     */
    std::shared_ptr<StaticPolyline> getPolyRepresentationForGeneration() const;

    /**
     * Computes the relative T value of the ground for the given x-pos. This will be the left T value of the spline
     * which covers the x-pos first.
     *
     * @param minX
     * @return
     */
    floatType getMinT(floatType minX);

    /**
     * Computes the relative T value of the ground for the given x-pos. This will be the right T value of the spline
     * which covers the x-pos last.
     *
     * @param maxX
     * @return
     */
    floatType getMaxT(floatType maxX);

    std::shared_ptr<std::vector<Vector>> getDerivatives() const;
    floatType getEndT() const;

    /**
     * Removes the last count many basepoints and adjusts the ground accordingly.
     *
     * @param count
     */
    void removeLastBasepoints(int count);

  private:
    std::shared_ptr<std::vector<Vector>> derivatives;
    std::vector<std::unique_ptr<TwoDimensionalHermiteSpline>> splines = {};
};

#endif // SURVIVING_SARNTAL_GROUND_HPP
