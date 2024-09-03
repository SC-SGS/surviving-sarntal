//
// Created by felix-roehr on 9/1/24.
//

#include "Line.hpp"

std::optional<Intersection> Line::calculateIntersection(const Line &other) const {
    floatType denominator = (this->start.x - this->end.x) * (other.start.y - other.end.y) -
                            (this->start.y - this->end.y) * (other.start.x - other.end.x);
    if (denominator == 0) {
        return std::nullopt;
    } else {
        floatType tThis = ((this->start.x - other.start.x) * (other.start.y - other.end.y) -
                           (this->start.y - other.start.y) * (other.start.x - other.end.x)) /
                          denominator;
        floatType tOther = -((this->start.x - this->end.x) * (this->start.y - other.start.y) -
                             (this->start.y - this->end.y) * (this->start.x - other.start.x)) /
                           denominator;

        if (0 <= tThis && tThis <= 1 && 0 <= tOther && tOther <= 1) {
            floatType intersectionX = this->start.x + tThis * (this->end.x - this->start.x);
            floatType intersectionY = this->start.y + tThis * (this->end.y - this->start.y);
            return Intersection{{intersectionX, intersectionY}, this->calculateNormal()};
        } else {
            return std::nullopt;
        }
    }
}

Vector Line::calculateNormal() const {
    Vector delta = this->end - this->start;
    return delta.rotateByAngle(PI / 2.0f).normalize();
}