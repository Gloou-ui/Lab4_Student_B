#include "PolynomialSolver.h"

#include <cmath>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void expectStatus(PolynomialStatus status, const char* operation) {
    if (status != POLYNOMIAL_STATUS_OK) {
        throw std::runtime_error(
            std::string(operation) + " failed with status " +
            std::to_string(static_cast<int>(status))
        );
    }
}

void expectNear(double actual, double expected, double tolerance = 1e-8) {
    if (std::abs(actual - expected) > tolerance) {
        throw std::runtime_error("A numerical check failed.");
    }
}

std::vector<double> getRoots(
    const double* coefficients,
    size_t coefficientCount,
    double tolerance = 1e-10
) {
    double roots[16]{};
    size_t rootCount = 0;
    expectStatus(
        polynomial_find_roots(
            coefficients,
            coefficientCount,
            tolerance,
            roots,
            sizeof(roots) / sizeof(roots[0]),
            &rootCount
        ),
        "polynomial_find_roots"
    );
    return std::vector<double>(roots, roots + rootCount);
}

void expectRoots(
    const std::vector<double>& actual,
    const std::vector<double>& expected
) {
    if (actual.size() != expected.size()) {
        throw std::runtime_error("Unexpected number of real roots.");
    }
    for (size_t i = 0; i < expected.size(); ++i) {
        expectNear(actual[i], expected[i]);
    }
}

void printRoots(const std::vector<double>& roots) {
    if (roots.empty()) {
        std::cout << "no real roots";
        return;
    }
    for (size_t i = 0; i < roots.size(); ++i) {
        if (i != 0) {
            std::cout << ", ";
        }
        std::cout << roots[i];
    }
}

}  // namespace

int main() {
    try {
        const double cubic[]{-6.0, 11.0, -6.0, 1.0};
        double valueAtTwo = 0.0;
        expectStatus(
            polynomial_evaluate(cubic, 4, 2.0, &valueAtTwo),
            "polynomial_evaluate"
        );
        expectNear(valueAtTwo, 0.0);

        double cubicDerivative[4]{};
        size_t derivativeCount = 0;
        expectStatus(
            polynomial_derivative(
                cubic,
                4,
                cubicDerivative,
                sizeof(cubicDerivative) / sizeof(cubicDerivative[0]),
                &derivativeCount
            ),
            "polynomial_derivative"
        );
        double derivativeAtTwo = 0.0;
        expectStatus(
            polynomial_evaluate(
                cubicDerivative,
                derivativeCount,
                2.0,
                &derivativeAtTwo
            ),
            "polynomial_evaluate(derivative)"
        );
        expectNear(derivativeAtTwo, -1.0);

        double smallDerivativeBuffer[1]{};
        size_t requiredDerivativeCount = 0;
        if (polynomial_derivative(
                cubic,
                4,
                smallDerivativeBuffer,
                1,
                &requiredDerivativeCount
            ) != POLYNOMIAL_STATUS_BUFFER_TOO_SMALL ||
            requiredDerivativeCount != 3) {
            throw std::runtime_error(
                "The derivative buffer-size check failed."
            );
        }

        double newtonResult = 0.0;
        expectStatus(
            polynomial_newton_root(
                cubic,
                4,
                2.8,
                1e-10,
                100,
                &newtonResult
            ),
            "polynomial_newton_root"
        );
        expectNear(newtonResult, 3.0);

        const std::vector<double> cubicRoots = getRoots(cubic, 4);
        expectRoots(cubicRoots, {1.0, 2.0, 3.0});
        double smallRootBuffer[2]{};
        size_t requiredRootCount = 0;
        if (polynomial_find_roots(
                cubic,
                4,
                1e-10,
                smallRootBuffer,
                2,
                &requiredRootCount
            ) != POLYNOMIAL_STATUS_BUFFER_TOO_SMALL ||
            requiredRootCount != 3) {
            throw std::runtime_error("The root buffer-size check failed.");
        }

        const double repeatedRoot[]{2.0, -3.0, 0.0, 1.0};
        const std::vector<double> repeatedRoots = getRoots(repeatedRoot, 4);
        expectRoots(repeatedRoots, {-2.0, 1.0});

        const double noRealRootPolynomial[]{1.0, 0.0, 1.0};
        const std::vector<double> noRealRoots =
            getRoots(noRealRootPolynomial, 3);
        expectRoots(noRealRoots, {});

        const double linear[]{-4.0, 2.0, 0.0};
        const std::vector<double> linearRoots = getRoots(linear, 3);
        expectRoots(linearRoots, {2.0});

        const double zeroPolynomial[]{0.0, 0.0, 0.0};
        double ignoredRoots[4]{};
        size_t ignoredRootCount = 0;
        const PolynomialStatus zeroPolynomialStatus = polynomial_find_roots(
            zeroPolynomial,
            3,
            1e-10,
            ignoredRoots,
            4,
            &ignoredRootCount
        );
        if (zeroPolynomialStatus != POLYNOMIAL_STATUS_INVALID_ARGUMENT) {
            throw std::runtime_error("The zero polynomial was not rejected.");
        }

        std::cout << "Polynomial: x^3 - 6x^2 + 11x - 6\n";
        std::cout << "polynomial_evaluate(p, 2) = " << valueAtTwo << '\n';
        std::cout << "polynomial_derivative(p), evaluated at 2 = "
                  << derivativeAtTwo << '\n';
        std::cout << "polynomial_newton_root(p, 2.8) = "
                  << newtonResult << '\n';
        std::cout << "polynomial_find_roots(p) = ";
        printRoots(cubicRoots);
        std::cout << "\n\n";

        std::cout << "Polynomial: (x - 1)^2 * (x + 2)\n";
        std::cout << "polynomial_find_roots(p) = ";
        printRoots(repeatedRoots);
        std::cout << "\n\n";

        std::cout << "Polynomial: x^2 + 1\n";
        std::cout << "polynomial_find_roots(p) = ";
        printRoots(noRealRoots);
        std::cout << "\n\n";

        std::cout << "Polynomial: 2x - 4\n";
        std::cout << "polynomial_find_roots(p) = ";
        printRoots(linearRoots);
        std::cout << "\n\nAll polynomial solver checks passed.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Polynomial solver check failed: " << error.what() << '\n';
        return 1;
    }
}
