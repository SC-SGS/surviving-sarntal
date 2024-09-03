//
// Created by felix-roehr on 7/6/24.
//

#ifndef SURVIVING_SARNTAL_POLYNOMIAL_HPP
#define SURVIVING_SARNTAL_POLYNOMIAL_HPP

#include "../../game/GameProperties.hpp"
#include <vector>

/**
 * This class represents a polynomial. It stores the coefficients in the order x^k, x^(k-1), ..., x^2, x, 1.
 */
class Polynomial {
  public:
    explicit Polynomial(std::vector<floatType> coefficients);
    ~Polynomial() = default;

    /**
     * Evaluates this Polynomial using the Horner-Schema.
     *
     * @param xPos
     * @return corresponding yPos
     */
    floatType evaluate(floatType xPos) const;

    /**
     * Evaluates the first evaluateDerivative of this polynomial at the given xPos.
     *
     * @param xPos
     * @return corresponding value of the evaluateDerivative
     */
    floatType evaluateDerivative(floatType xPos) const;

    /**
     * Evaluates the n-th evaluateDerivative of this polynomial at the given xPos.
     *
     * @param xPos
     * @param n
     * @return corresponding value of the evaluateDerivative
     */
    floatType evaluateDerivative(floatType xPos, int n) const;

    /**
     * Copy constructor
     *
     * @param toCopy
     */
    Polynomial(const Polynomial &toCopy);

  private:
    std::vector<floatType> coefficients;

    Polynomial getDerivative() const;

    Polynomial getDerivative(int n) const;
};

#endif // SURVIVING_SARNTAL_POLYNOMIAL_HPP
