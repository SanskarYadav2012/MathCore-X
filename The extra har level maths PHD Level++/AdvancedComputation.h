#pragma once

#include "MathCore.h"

namespace MathEngine::AdvancedComputation
{
class Matrix
{
public:
    Matrix() = default;
    Matrix(std::size_t rows, std::size_t columns, long double initialValue = 0.0L);

    [[nodiscard]] static Matrix identity(std::size_t dimension);

    [[nodiscard]] std::size_t rows() const noexcept { return rows_; }
    [[nodiscard]] std::size_t columns() const noexcept { return columns_; }
    [[nodiscard]] bool isSquare() const noexcept { return rows_ == columns_; }

    [[nodiscard]] long double operator()(std::size_t row, std::size_t column) const;
    long double& operator()(std::size_t row, std::size_t column);

    [[nodiscard]] std::vector<long double> column(std::size_t index) const;
    void swapColumns(std::size_t first, std::size_t second);

private:
    [[nodiscard]] std::size_t offset(std::size_t row, std::size_t column) const;

    std::size_t rows_ {};
    std::size_t columns_ {};
    std::vector<long double> data_ {};
};

struct EigenPair
{
    long double eigenvalue {};
    std::vector<long double> eigenvector {};
};

struct EigenDecomposition
{
    std::vector<long double> eigenvalues {};
    Matrix eigenvectors {};
};

[[nodiscard]] std::vector<std::complex<long double>> fft(std::vector<std::complex<long double>> values, bool inverse = false);
[[nodiscard]] std::vector<std::complex<long double>> ifft(std::vector<std::complex<long double>> values);

[[nodiscard]] long double evaluatePowerSeries(const std::vector<long double>& coefficients, long double x);
[[nodiscard]] long double taylorApproximation(const std::vector<long double>& derivativesAtCenter, long double center, long double x);
[[nodiscard]] long double maclaurinExp(long double x, std::size_t terms = 12U);
[[nodiscard]] long double maclaurinSin(long double x, std::size_t terms = 10U);
[[nodiscard]] long double maclaurinCos(long double x, std::size_t terms = 10U);

struct ODEPoint
{
    long double t {};
    std::vector<long double> state {};
};

class ODESolver
{
public:
    using State = std::vector<long double>;
    using System = std::function<State(long double, const State&)>;

    [[nodiscard]] static std::vector<ODEPoint> euler(System system, long double t0, State initialState,
        long double stepSize, std::size_t steps);

    [[nodiscard]] static std::vector<ODEPoint> rungeKutta4(System system, long double t0, State initialState,
        long double stepSize, std::size_t steps);
};

[[nodiscard]] EigenPair powerIteration(const Matrix& matrix, std::size_t maxIterations = 500U, long double tolerance = 1.0e-10L);
[[nodiscard]] EigenDecomposition jacobiEigenDecomposition(const Matrix& symmetricMatrix,
    std::size_t maxIterations = 100U, long double tolerance = 1.0e-12L);

[[nodiscard]] long double computePi(std::size_t iterations = 5U);
[[nodiscard]] long double computeGoldenRatio(std::size_t iterations = 80U);
} // namespace MathEngine::AdvancedComputation

