//
// Created by felix-roehr on 7/6/24.
//

#include "Polynomial.hpp"
#include "spdlog/spdlog.h"

#include <utility>

Polynomial::Polynomial(std::vector<floatType> coefficients) : coefficients(std::move(coefficients)) {}

floatType Polynomial::evaluate(floatType xPos) const {
    floatType result = 0;
    for (floatType coeff : this->coefficients) {
        // spdlog::info("Current result: {}", result);
        result *= xPos;
        result += coeff;
    }
    return result;
}

floatType Polynomial::evaluateDerivative(floatType xPos) const { return this->evaluateDerivative(xPos, 1); }

floatType Polynomial::evaluateDerivative(floatType xPos, int n) const {
    Polynomial pol = this->getDerivative(n);
    return pol.evaluate(xPos);
}

Polynomial Polynomial::getDerivative() const {
    std::vector<floatType> coeffs = {};
    if (this->coefficients.size() == 1) {
        coeffs.push_back(0);
    } else {
        for (int i = 0; i < this->coefficients.size() - 1; i++) {
            auto exp = static_cast<floatType>(this->coefficients.size() - 1 - i);
            floatType prevCoeff = this->coefficients.at(i);
            coeffs.push_back(exp * prevCoeff);
        }
    }

    return Polynomial(coeffs);
}

Polynomial Polynomial::getDerivative(int n) const {
    Polynomial pol(*this);
    for (int i = 0; i < n; i++) {
        pol = pol.getDerivative();
    }
    return pol;
}

Polynomial::Polynomial(const Polynomial &toCopy) {
    std::vector<floatType> coeffs(toCopy.coefficients);
    this->coefficients = coeffs;
}
