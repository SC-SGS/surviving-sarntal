//
// Created by felix-roehr on 7/26/24.
//

#ifndef SURVIVING_SARNTAL_LINE_HPP
#define SURVIVING_SARNTAL_LINE_HPP

#include "../../utilities/vector.h"
#include "Intersection.hpp"

struct Line {
    Vector start;
    Vector end;

    /**
     * Calculates the intersection between this line and the other line.
     *
     * @param other
     * @return intersection if there is one, nullopt else
     */
    std::optional<Intersection> calculateIntersection(const Line &other) const;

    /**
     * Calculates the vector with length one that is orthogonal to this line
     *
     * @return
     */
    Vector calculateNormal() const;
};

#endif // SURVIVING_SARNTAL_LINE_HPP
