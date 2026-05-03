#pragma once

#include "MathCore.h"

namespace MathEngine::AlgebraStats
{
struct RootSet
{
    std::vector<std::complex<long double>> roots {};
};

struct QuadraticFactorization
{
    std::string expression {};
    bool factorsOverReals {};
    bool factorsOverIntegers {};
};

struct CheckedUnsigned64
{
    std::uint64_t value {};
    bool overflowed {};
};

RootSet solveQuadratic(long double a, long double b, long double c);
RootSet solveCubic(long double a, long double b, long double c, long double d);
QuadraticFactorization factorQuadratic(long double a, long double b, long double c);

CheckedUnsigned64 permutations(std::uint64_t n, std::uint64_t r);
CheckedUnsigned64 combinations(std::uint64_t n, std::uint64_t r);

class Probability
{
public:
    static long double conditional(long double intersectionAB, long double probabilityB);
    static long double bayes(long double probabilityBGivenA, long double probabilityA, long double probabilityB);
    static long double joint(long double probabilityBGivenA, long double probabilityA);
    static long double totalProbability(const std::vector<long double>& priors, const std::vector<long double>& likelihoods);
};

template <typename T>
struct ModeResult
{
    std::vector<T> values {};
    std::size_t frequency {};
};

template <typename T>
[[nodiscard]] long double mean(const std::vector<T>& data)
{
    if (data.empty())
    {
        throw std::invalid_argument("Mean is undefined for an empty data set.");
    }

    // Long-double accumulation minimizes cancellation when large vectors mix scales.
    const auto total = std::accumulate(data.begin(), data.end(), 0.0L,
        [](const long double running, const T value)
        {
            return running + static_cast<long double>(value);
        });

    return total / static_cast<long double>(data.size());
}

template <typename T>
[[nodiscard]] long double median(std::vector<T> data)
{
    if (data.empty())
    {
        throw std::invalid_argument("Median is undefined for an empty data set.");
    }

    const auto mid = data.begin() + static_cast<std::ptrdiff_t>(data.size() / 2U);
    std::nth_element(data.begin(), mid, data.end());

    if (data.size() % 2U == 1U)
    {
        return static_cast<long double>(*mid);
    }

    const auto lower = *std::max_element(data.begin(), mid);
    return (static_cast<long double>(lower) + static_cast<long double>(*mid)) / 2.0L;
}

template <typename T>
[[nodiscard]] ModeResult<T> mode(const std::vector<T>& data)
{
    if (data.empty())
    {
        throw std::invalid_argument("Mode is undefined for an empty data set.");
    }

    std::size_t maxFrequency = 0;
    ModeResult<T> result {};

    if constexpr (std::is_integral_v<T>)
    {
        std::unordered_map<T, std::size_t> frequencies;
        frequencies.reserve(data.size());

        for (const auto value : data)
        {
            const auto frequency = ++frequencies[value];
            if (frequency > maxFrequency)
            {
                maxFrequency = frequency;
            }
        }

        for (const auto& [value, frequency] : frequencies)
        {
            if (frequency == maxFrequency)
            {
                result.values.push_back(value);
            }
        }
    }
    else
    {
        // Floating-point data is grouped by exact value here; callers can layer bucketization above this.
        std::map<T, std::size_t> frequencies;
        for (const auto value : data)
        {
            const auto frequency = ++frequencies[value];
            if (frequency > maxFrequency)
            {
                maxFrequency = frequency;
            }
        }

        for (const auto& [value, frequency] : frequencies)
        {
            if (frequency == maxFrequency)
            {
                result.values.push_back(value);
            }
        }
    }

    std::sort(result.values.begin(), result.values.end());
    result.frequency = maxFrequency;
    return result;
}
} // namespace MathEngine::AlgebraStats

