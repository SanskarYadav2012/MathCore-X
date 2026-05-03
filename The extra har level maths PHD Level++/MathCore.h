#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <limits>
#include <map>
#include <memory>
#include <numbers>
#include <numeric>
#include <optional>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace MathEngine
{
inline constexpr long double kEpsilon = 1.0e-12L;

template <typename T>
constexpr T square(const T value) noexcept
{
    return value * value;
}

template <typename T>
constexpr T cube(const T value) noexcept
{
    return value * value * value;
}

inline long double sanitizeZero(const long double value, const long double epsilon = kEpsilon) noexcept
{
    return std::abs(value) < epsilon ? 0.0L : value;
}

struct Vector2
{
    long double x {};
    long double y {};

    [[nodiscard]] std::string toString() const;
};

struct Vector3
{
    long double x {};
    long double y {};
    long double z {};

    [[nodiscard]] std::string toString() const;
};

[[nodiscard]] long double dot(const Vector2& lhs, const Vector2& rhs) noexcept;
[[nodiscard]] long double dot(const Vector3& lhs, const Vector3& rhs) noexcept;
[[nodiscard]] long double norm(const Vector2& value) noexcept;
[[nodiscard]] long double norm(const Vector3& value) noexcept;
[[nodiscard]] Vector3 normalize(const Vector3& value);

[[nodiscard]] std::string formatReal(long double value, int precision = 10);
[[nodiscard]] std::string formatComplex(const std::complex<long double>& value, int precision = 10);
[[nodiscard]] std::string formatCoordinateTerm(char variable, long double offset);

template <typename IntegerT>
[[nodiscard]] constexpr bool isAlmostInteger(const long double value, const long double tolerance = 1.0e-10L) noexcept
{
    static_assert(std::is_integral_v<IntegerT>, "IntegerT must be integral.");
    const auto rounded = std::llround(value);
    return std::abs(value - static_cast<long double>(rounded)) <= tolerance;
}

namespace Detail
{
template <typename T>
[[nodiscard]] constexpr T checkedAdd(const T lhs, const T rhs)
{
    if constexpr (std::is_unsigned_v<T>)
    {
        if (lhs > (std::numeric_limits<T>::max)() - rhs)
        {
            throw std::overflow_error("Integer addition overflow.");
        }
    }

    return static_cast<T>(lhs + rhs);
}

template <typename T>
[[nodiscard]] constexpr T checkedMultiply(const T lhs, const T rhs)
{
    if constexpr (std::is_unsigned_v<T>)
    {
        if (lhs != 0 && rhs > (std::numeric_limits<T>::max)() / lhs)
        {
            throw std::overflow_error("Integer multiplication overflow.");
        }
    }

    return static_cast<T>(lhs * rhs);
}
} // namespace Detail
} // namespace MathEngine
