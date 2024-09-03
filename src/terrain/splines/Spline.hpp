//
// Created by felix-roehr on 7/26/24.
//

#ifndef SURVIVING_SARNTAL_SPLINE_HPP
#define SURVIVING_SARNTAL_SPLINE_HPP

#include "../../game/GameProperties.hpp"
template <typename U> class Spline {
  public:
    Spline(floatType leftT, floatType rightT) : leftT(leftT), rightT(rightT) {}
    virtual ~Spline() = default;

    /**
     * Evaluates this spline piece at the given relative parameter t
     *
     * @param relativeT
     * @return corresponding value of the spline
     */
    U evaluate(floatType relativeT) const { return this->evaluateDerivative(relativeT, 0); }

    /**
     * Evaluates the first evaluateDerivative of this spline piece at the given relativeT.
     *
     * @param xPos
     * @return corresponding value of the evaluateDerivative
     */
    U evaluateDerivative(floatType relativeT) const { return this->evaluateDerivative(relativeT, 1); }

    /**
     * Evaluates the n-th evaluateDerivative of this spline piece at the given relativeT.
     *
     * @param relativeT
     * @param n
     * @return corresponding value of the evaluateDerivative
     */
    virtual U evaluateDerivative(floatType relativeT, int n) const = 0;

    /**
     * Returns whether the given t value is in range of this spline.
     *
     * @param relativeT
     * @return
     */
    bool isInRange(floatType relativeT) const { return relativeT >= this->leftT && relativeT <= rightT; }

    U getLeftPos() const { return this->leftPos; }
    U getRightPos() const { return this->rightPos; };
    floatType getLeftT() const { return leftT; }
    floatType getRightT() const { return rightT; };

  protected:
    floatType leftT;
    floatType rightT;
    U leftPos;
    U rightPos;
};

#endif // SURVIVING_SARNTAL_SPLINE_HPP
