#include "pch.h"

#include "AdvancedComputation.h"

namespace MathEngine::AdvancedComputation
{
namespace
{
std::size_t nextPowerOfTwo(std::size_t value)
{
    std::size_t power = 1U;
    while (power < value)
    {
        power <<= 1U;
    }

    return power;
}

long double factorial(const std::size_t n)
{
    long double value = 1.0L;
    for (std::size_t i = 2; i <= n; ++i)
    {
        value *= static_cast<long double>(i);
    }

    return value;
}

std::vector<long double> normalizeVector(std::vector<long double> vector)
{
    const auto length = std::sqrt(std::inner_product(vector.begin(), vector.end(), vector.begin(), 0.0L));
    if (length < kEpsilon)
    {
        throw std::invalid_argument("Cannot normalize the zero vector.");
    }

    for (auto& value : vector)
    {
        value /= length;
    }

    return vector;
}

std::vector<long double> multiply(const Matrix& matrix, const std::vector<long double>& vector)
{
    if (matrix.columns() != vector.size())
    {
        throw std::invalid_argument("Matrix/vector dimensions do not match.");
    }

    std::vector<long double> result(matrix.rows(), 0.0L);
    for (std::size_t row = 0; row < matrix.rows(); ++row)
    {
        for (std::size_t column = 0; column < matrix.columns(); ++column)
        {
            result[row] += matrix(row, column) * vector[column];
        }
    }

    return result;
}

std::vector<long double> addScaled(const std::vector<long double>& lhs, const std::vector<long double>& rhs, const long double scale)
{
    if (lhs.size() != rhs.size())
    {
        throw std::invalid_argument("Vector dimensions do not match.");
    }

    std::vector<long double> result(lhs.size(), 0.0L);
    for (std::size_t index = 0; index < lhs.size(); ++index)
    {
        result[index] = lhs[index] + scale * rhs[index];
    }

    return result;
}
} // namespace

Matrix::Matrix(const std::size_t rows, const std::size_t columns, const long double initialValue)
    : rows_(rows), columns_(columns), data_(rows * columns, initialValue)
{
    if (rows == 0U || columns == 0U)
    {
        throw std::invalid_argument("Matrix dimensions must be strictly positive.");
    }
}

Matrix Matrix::identity(const std::size_t dimension)
{
    Matrix result(dimension, dimension, 0.0L);
    for (std::size_t index = 0; index < dimension; ++index)
    {
        result(index, index) = 1.0L;
    }

    return result;
}

long double Matrix::operator()(const std::size_t row, const std::size_t column) const
{
    return data_.at(offset(row, column));
}

long double& Matrix::operator()(const std::size_t row, const std::size_t column)
{
    return data_.at(offset(row, column));
}

std::vector<long double> Matrix::column(const std::size_t index) const
{
    if (index >= columns_)
    {
        throw std::out_of_range("Matrix column index is out of range.");
    }

    std::vector<long double> result(rows_, 0.0L);
    for (std::size_t row = 0; row < rows_; ++row)
    {
        result[row] = (*this)(row, index);
    }

    return result;
}

void Matrix::swapColumns(const std::size_t first, const std::size_t second)
{
    for (std::size_t row = 0; row < rows_; ++row)
    {
        std::swap((*this)(row, first), (*this)(row, second));
    }
}

std::size_t Matrix::offset(const std::size_t row, const std::size_t column) const
{
    if (row >= rows_ || column >= columns_)
    {
        throw std::out_of_range("Matrix indices are out of range.");
    }

    return row * columns_ + column;
}

std::vector<std::complex<long double>> fft(std::vector<std::complex<long double>> values, const bool inverse)
{
    if (values.empty())
    {
        return {};
    }

    const auto paddedSize = nextPowerOfTwo(values.size());
    values.resize(paddedSize, { 0.0L, 0.0L });

    // Bit-reversal permutation groups the inputs so each butterfly stage accesses contiguous blocks.
    for (std::size_t index = 1U, reversed = 0U; index < paddedSize; ++index)
    {
        std::size_t bit = paddedSize >> 1U;
        for (; (reversed & bit) != 0U; bit >>= 1U)
        {
            reversed ^= bit;
        }

        reversed ^= bit;
        if (index < reversed)
        {
            std::swap(values[index], values[reversed]);
        }
    }

    const auto sign = inverse ? 1.0L : -1.0L;
    for (std::size_t blockSize = 2U; blockSize <= paddedSize; blockSize <<= 1U)
    {
        const auto angle = sign * 2.0L * std::numbers::pi_v<long double> / static_cast<long double>(blockSize);
        const std::complex<long double> root = std::polar(1.0L, angle);

        for (std::size_t block = 0U; block < paddedSize; block += blockSize)
        {
            std::complex<long double> twiddle(1.0L, 0.0L);
            for (std::size_t offset = 0U; offset < blockSize / 2U; ++offset)
            {
                const auto even = values[block + offset];
                const auto odd = twiddle * values[block + offset + blockSize / 2U];

                values[block + offset] = even + odd;
                values[block + offset + blockSize / 2U] = even - odd;
                twiddle *= root;
            }
        }
    }

    if (inverse)
    {
        for (auto& value : values)
        {
            value /= static_cast<long double>(paddedSize);
        }
    }

    return values;
}

std::vector<std::complex<long double>> ifft(std::vector<std::complex<long double>> values)
{
    return fft(std::move(values), true);
}

long double evaluatePowerSeries(const std::vector<long double>& coefficients, const long double x)
{
    long double result = 0.0L;
    for (auto iterator = coefficients.rbegin(); iterator != coefficients.rend(); ++iterator)
    {
        result = std::fma(result, x, *iterator);
    }

    return result;
}

long double taylorApproximation(const std::vector<long double>& derivativesAtCenter, const long double center, const long double x)
{
    if (derivativesAtCenter.empty())
    {
        throw std::invalid_argument("Taylor approximation requires at least the function value at the expansion point.");
    }

    std::vector<long double> coefficients;
    coefficients.reserve(derivativesAtCenter.size());

    // f^(n)(a) / n! are the Taylor coefficients multiplying (x - a)^n.
    for (std::size_t order = 0; order < derivativesAtCenter.size(); ++order)
    {
        coefficients.push_back(derivativesAtCenter[order] / factorial(order));
    }

    return evaluatePowerSeries(coefficients, x - center);
}

long double maclaurinExp(const long double x, const std::size_t terms)
{
    std::vector<long double> coefficients(terms, 1.0L);
    for (std::size_t index = 0; index < coefficients.size(); ++index)
    {
        coefficients[index] = 1.0L / factorial(index);
    }

    return evaluatePowerSeries(coefficients, x);
}

long double maclaurinSin(const long double x, const std::size_t terms)
{
    long double result = 0.0L;
    for (std::size_t index = 0; index < terms; ++index)
    {
        const auto order = 2U * index + 1U;
        const auto sign = index % 2U == 0U ? 1.0L : -1.0L;
        result += sign * std::pow(x, static_cast<long double>(order)) / factorial(order);
    }

    return result;
}

long double maclaurinCos(const long double x, const std::size_t terms)
{
    long double result = 0.0L;
    for (std::size_t index = 0; index < terms; ++index)
    {
        const auto order = 2U * index;
        const auto sign = index % 2U == 0U ? 1.0L : -1.0L;
        result += sign * std::pow(x, static_cast<long double>(order)) / factorial(order);
    }

    return result;
}

std::vector<ODEPoint> ODESolver::euler(System system, long double t0, State initialState,
    const long double stepSize, const std::size_t steps)
{
    if (stepSize == 0.0L)
    {
        throw std::invalid_argument("Euler solver requires a non-zero step size.");
    }

    std::vector<ODEPoint> trajectory;
    trajectory.reserve(steps + 1U);

    auto state = std::move(initialState);
    long double time = t0;
    trajectory.push_back({ time, state });

    for (std::size_t step = 0; step < steps; ++step)
    {
        const auto derivative = system(time, state);
        state = addScaled(state, derivative, stepSize);
        time += stepSize;
        trajectory.push_back({ time, state });
    }

    return trajectory;
}

std::vector<ODEPoint> ODESolver::rungeKutta4(System system, long double t0, State initialState,
    const long double stepSize, const std::size_t steps)
{
    if (stepSize == 0.0L)
    {
        throw std::invalid_argument("Runge-Kutta solver requires a non-zero step size.");
    }

    std::vector<ODEPoint> trajectory;
    trajectory.reserve(steps + 1U);

    auto state = std::move(initialState);
    long double time = t0;
    trajectory.push_back({ time, state });

    for (std::size_t step = 0; step < steps; ++step)
    {
        // RK4 samples the slope at the beginning, two midpoints, and the end of the interval.
        const auto k1 = system(time, state);
        const auto k2 = system(time + stepSize / 2.0L, addScaled(state, k1, stepSize / 2.0L));
        const auto k3 = system(time + stepSize / 2.0L, addScaled(state, k2, stepSize / 2.0L));
        const auto k4 = system(time + stepSize, addScaled(state, k3, stepSize));

        for (std::size_t index = 0; index < state.size(); ++index)
        {
            state[index] += stepSize * (k1[index] + 2.0L * k2[index] + 2.0L * k3[index] + k4[index]) / 6.0L;
        }

        time += stepSize;
        trajectory.push_back({ time, state });
    }

    return trajectory;
}

EigenPair powerIteration(const Matrix& matrix, const std::size_t maxIterations, const long double tolerance)
{
    if (!matrix.isSquare())
    {
        throw std::invalid_argument("Power iteration requires a square matrix.");
    }

    std::vector<long double> vector(matrix.columns(), 1.0L);
    vector = normalizeVector(std::move(vector));

    long double eigenvalue = 0.0L;

    for (std::size_t iteration = 0; iteration < maxIterations; ++iteration)
    {
        auto next = multiply(matrix, vector);
        next = normalizeVector(std::move(next));

        const auto multiplied = multiply(matrix, next);
        const auto nextEigenvalue = std::inner_product(next.begin(), next.end(), multiplied.begin(), 0.0L);

        if (std::abs(nextEigenvalue - eigenvalue) < tolerance)
        {
            return { nextEigenvalue, std::move(next) };
        }

        vector = std::move(next);
        eigenvalue = nextEigenvalue;
    }

    return { eigenvalue, std::move(vector) };
}

EigenDecomposition jacobiEigenDecomposition(const Matrix& symmetricMatrix,
    const std::size_t maxIterations, const long double tolerance)
{
    if (!symmetricMatrix.isSquare())
    {
        throw std::invalid_argument("Jacobi eigen decomposition requires a square matrix.");
    }

    const auto dimension = symmetricMatrix.rows();
    Matrix matrix = symmetricMatrix;
    Matrix eigenvectors = Matrix::identity(dimension);

    for (std::size_t iteration = 0; iteration < maxIterations; ++iteration)
    {
        std::size_t p = 0U;
        std::size_t q = 1U;
        long double maxOffDiagonal = 0.0L;

        for (std::size_t row = 0; row < dimension; ++row)
        {
            for (std::size_t column = row + 1U; column < dimension; ++column)
            {
                const auto value = std::abs(matrix(row, column));
                if (value > maxOffDiagonal)
                {
                    maxOffDiagonal = value;
                    p = row;
                    q = column;
                }
            }
        }

        if (maxOffDiagonal < tolerance)
        {
            break;
        }

        // Each Jacobi rotation zeroes the current largest off-diagonal element while preserving symmetry.
        const auto theta = 0.5L * std::atan2(2.0L * matrix(p, q), matrix(q, q) - matrix(p, p));
        const auto cosine = std::cos(theta);
        const auto sine = std::sin(theta);

        const auto app = matrix(p, p);
        const auto aqq = matrix(q, q);
        const auto apq = matrix(p, q);

        matrix(p, p) = cosine * cosine * app - 2.0L * sine * cosine * apq + sine * sine * aqq;
        matrix(q, q) = sine * sine * app + 2.0L * sine * cosine * apq + cosine * cosine * aqq;
        matrix(p, q) = 0.0L;
        matrix(q, p) = 0.0L;

        for (std::size_t index = 0; index < dimension; ++index)
        {
            if (index == p || index == q)
            {
                continue;
            }

            const auto aip = matrix(index, p);
            const auto aiq = matrix(index, q);
            matrix(index, p) = cosine * aip - sine * aiq;
            matrix(p, index) = matrix(index, p);
            matrix(index, q) = sine * aip + cosine * aiq;
            matrix(q, index) = matrix(index, q);
        }

        for (std::size_t row = 0; row < dimension; ++row)
        {
            const auto vip = eigenvectors(row, p);
            const auto viq = eigenvectors(row, q);
            eigenvectors(row, p) = cosine * vip - sine * viq;
            eigenvectors(row, q) = sine * vip + cosine * viq;
        }
    }

    std::vector<long double> eigenvalues(dimension, 0.0L);
    for (std::size_t index = 0; index < dimension; ++index)
    {
        eigenvalues[index] = matrix(index, index);
    }

    std::vector<std::size_t> order(dimension, 0U);
    std::iota(order.begin(), order.end(), 0U);
    std::sort(order.begin(), order.end(),
        [&](const std::size_t lhs, const std::size_t rhs)
        {
            return eigenvalues[lhs] > eigenvalues[rhs];
        });

    std::vector<long double> sortedEigenvalues;
    sortedEigenvalues.reserve(dimension);
    Matrix sortedEigenvectors(dimension, dimension, 0.0L);

    for (std::size_t newIndex = 0; newIndex < order.size(); ++newIndex)
    {
        const auto oldIndex = order[newIndex];
        sortedEigenvalues.push_back(eigenvalues[oldIndex]);
        for (std::size_t row = 0; row < dimension; ++row)
        {
            sortedEigenvectors(row, newIndex) = eigenvectors(row, oldIndex);
        }
    }

    return { std::move(sortedEigenvalues), std::move(sortedEigenvectors) };
}

long double computePi(const std::size_t iterations)
{
    // Gauss-Legendre converges quadratically, making it practical even with long double precision.
    long double a = 1.0L;
    long double b = 1.0L / std::sqrt(2.0L);
    long double t = 0.25L;
    long double p = 1.0L;

    for (std::size_t iteration = 0; iteration < iterations; ++iteration)
    {
        const auto nextA = (a + b) / 2.0L;
        const auto nextB = std::sqrt(a * b);
        const auto difference = a - nextA;
        const auto nextT = t - p * difference * difference;
        const auto nextP = 2.0L * p;

        a = nextA;
        b = nextB;
        t = nextT;
        p = nextP;
    }

    return square(a + b) / (4.0L * t);
}

long double computeGoldenRatio(const std::size_t iterations)
{
    long double value = 1.0L;
    for (std::size_t iteration = 0; iteration < iterations; ++iteration)
    {
        // phi = 1 + 1 / phi is the fixed-point equation behind the continued fraction.
        value = 1.0L + 1.0L / value;
    }

    return value;
}
} // namespace MathEngine::AdvancedComputation

