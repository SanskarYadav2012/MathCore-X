#include "pch.h"

#include "AlgebraStats.h"

namespace MathEngine::AlgebraStats
{
namespace
{
std::complex<long double> complexCubeRoot(const std::complex<long double>& value)
{
    if (std::abs(value) < kEpsilon)
    {
        return { 0.0L, 0.0L };
    }

    return std::pow(value, 1.0L / 3.0L);
}

std::complex<long double> sanitizeComplex(const std::complex<long double>& value)
{
    return { sanitizeZero(value.real()), sanitizeZero(value.imag()) };
}

void validateProbability(const long double probability, const std::string& name)
{
    if (probability < -kEpsilon || probability > 1.0L + kEpsilon)
    {
        throw std::out_of_range(name + " must lie in [0, 1].");
    }
}

std::string factorTerm(const std::complex<long double>& root)
{
    return "(x - (" + formatComplex(root) + "))";
}
} // namespace

RootSet solveQuadratic(const long double a, const long double b, const long double c)
{
    if (std::abs(a) < kEpsilon)
    {
        if (std::abs(b) < kEpsilon)
        {
            throw std::invalid_argument("At least one of a or b must be non-zero.");
        }

        return { { { -c / b, 0.0L } } };
    }

    const std::complex<long double> discriminant = square(b) - 4.0L * a * c;
    const auto sqrtDiscriminant = std::sqrt(discriminant);

    // Using q avoids catastrophic cancellation when b and sqrt(discriminant) have similar magnitude.
    const auto q = -0.5L * (b + (b >= 0.0L ? 1.0L : -1.0L) * sqrtDiscriminant);
    auto first = q / a;
    auto second = c / q;

    if (std::abs(q) < kEpsilon)
    {
        first = (-b + sqrtDiscriminant) / (2.0L * a);
        second = (-b - sqrtDiscriminant) / (2.0L * a);
    }

    return { { sanitizeComplex(first), sanitizeComplex(second) } };
}

RootSet solveCubic(const long double a, const long double b, const long double c, const long double d)
{
    if (std::abs(a) < kEpsilon)
    {
        return solveQuadratic(b, c, d);
    }

    // Cardano starts by depressing the cubic x = t - b / (3a), removing the quadratic term.
    const auto invA = 1.0L / a;
    const auto normalizedB = b * invA;
    const auto normalizedC = c * invA;
    const auto normalizedD = d * invA;

    const auto p = normalizedC - square(normalizedB) / 3.0L;
    const auto q = (2.0L * normalizedB * square(normalizedB)) / 27.0L
        - (normalizedB * normalizedC) / 3.0L
        + normalizedD;

    const std::complex<long double> delta = square(q / 2.0L) + cube(p / 3.0L);
    const auto sqrtDelta = std::sqrt(delta);
    const auto u = complexCubeRoot(-q / 2.0L + sqrtDelta);
    const auto v = complexCubeRoot(-q / 2.0L - sqrtDelta);

    const std::complex<long double> omega1(-0.5L, std::sqrt(3.0L) / 2.0L);
    const std::complex<long double> omega2(-0.5L, -std::sqrt(3.0L) / 2.0L);
    const std::complex<long double> shift = normalizedB / 3.0L;

    std::vector<std::complex<long double>> roots;
    roots.reserve(3);

    roots.emplace_back(sanitizeComplex(u + v - shift));
    roots.emplace_back(sanitizeComplex(omega1 * u + omega2 * v - shift));
    roots.emplace_back(sanitizeComplex(omega2 * u + omega1 * v - shift));

    return { std::move(roots) };
}

QuadraticFactorization factorQuadratic(const long double a, const long double b, const long double c)
{
    const auto roots = solveQuadratic(a, b, c);
    QuadraticFactorization result {};

    if (std::abs(a) < kEpsilon)
    {
        result.expression = formatReal(b) + factorTerm(roots.roots.front());
        result.factorsOverReals = true;
        result.factorsOverIntegers = false;
        return result;
    }

    result.expression = formatReal(a) + factorTerm(roots.roots[0]) + factorTerm(roots.roots[1]);
    result.factorsOverReals = std::abs(roots.roots[0].imag()) < kEpsilon && std::abs(roots.roots[1].imag()) < kEpsilon;

    if (result.factorsOverReals)
    {
        const auto ratio1 = roots.roots[0].real();
        const auto ratio2 = roots.roots[1].real();
        result.factorsOverIntegers = isAlmostInteger<long long>(ratio1) && isAlmostInteger<long long>(ratio2);
    }

    return result;
}

CheckedUnsigned64 permutations(const std::uint64_t n, const std::uint64_t r)
{
    if (r > n)
    {
        return { 0U, false };
    }

    try
    {
        std::uint64_t value = 1U;
        for (std::uint64_t i = 0; i < r; ++i)
        {
            value = Detail::checkedMultiply(value, n - i);
        }

        return { value, false };
    }
    catch (const std::overflow_error&)
    {
        return { 0U, true };
    }
}

CheckedUnsigned64 combinations(const std::uint64_t n, const std::uint64_t r)
{
    if (r > n)
    {
        return { 0U, false };
    }

    const auto k = (std::min)(r, n - r);
    std::vector<std::uint64_t> denominator;
    denominator.reserve(static_cast<std::size_t>(k));

    for (std::uint64_t divisor = 2; divisor <= k; ++divisor)
    {
        denominator.push_back(divisor);
    }

    std::vector<std::uint64_t> numerator;
    numerator.reserve(static_cast<std::size_t>(k));
    for (std::uint64_t value = n - k + 1U; value <= n; ++value)
    {
        numerator.push_back(value);
    }

    // Cancelling common factors before multiplication keeps exactness while minimizing overflow risk.
    for (auto& numeratorTerm : numerator)
    {
        for (auto& denominatorTerm : denominator)
        {
            const auto divisor = std::gcd(numeratorTerm, denominatorTerm);
            if (divisor > 1U)
            {
                numeratorTerm /= divisor;
                denominatorTerm /= divisor;
            }

            if (denominatorTerm == 1U)
            {
                continue;
            }
        }
    }

    try
    {
        std::uint64_t value = 1U;
        for (const auto term : numerator)
        {
            value = Detail::checkedMultiply(value, term);
        }

        return { value, false };
    }
    catch (const std::overflow_error&)
    {
        return { 0U, true };
    }
}

long double Probability::conditional(const long double intersectionAB, const long double probabilityB)
{
    validateProbability(intersectionAB, "P(A intersect B)");
    validateProbability(probabilityB, "P(B)");

    if (probabilityB <= kEpsilon)
    {
        throw std::domain_error("Conditional probability is undefined when the conditioning event has zero probability.");
    }

    return intersectionAB / probabilityB;
}

long double Probability::bayes(const long double probabilityBGivenA, const long double probabilityA, const long double probabilityB)
{
    validateProbability(probabilityBGivenA, "P(B|A)");
    validateProbability(probabilityA, "P(A)");
    validateProbability(probabilityB, "P(B)");

    if (probabilityB <= kEpsilon)
    {
        throw std::domain_error("Bayes' theorem is undefined when P(B) is zero.");
    }

    return (probabilityBGivenA * probabilityA) / probabilityB;
}

long double Probability::joint(const long double probabilityBGivenA, const long double probabilityA)
{
    validateProbability(probabilityBGivenA, "P(B|A)");
    validateProbability(probabilityA, "P(A)");
    return probabilityBGivenA * probabilityA;
}

long double Probability::totalProbability(const std::vector<long double>& priors, const std::vector<long double>& likelihoods)
{
    if (priors.size() != likelihoods.size() || priors.empty())
    {
        throw std::invalid_argument("Priors and likelihoods must be non-empty and share the same length.");
    }

    long double total = 0.0L;
    for (std::size_t index = 0; index < priors.size(); ++index)
    {
        validateProbability(priors[index], "Prior");
        validateProbability(likelihoods[index], "Likelihood");
        total += priors[index] * likelihoods[index];
    }

    return total;
}
} // namespace MathEngine::AlgebraStats
