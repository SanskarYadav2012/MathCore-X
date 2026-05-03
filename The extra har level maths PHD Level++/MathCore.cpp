#include "pch.h"

#include "MathCore.h"

namespace MathEngine
{
std::string Vector2::toString() const
{
    std::ostringstream stream;
    stream << "(" << formatReal(x) << ", " << formatReal(y) << ")";
    return stream.str();
}

std::string Vector3::toString() const
{
    std::ostringstream stream;
    stream << "(" << formatReal(x) << ", " << formatReal(y) << ", " << formatReal(z) << ")";
    return stream.str();
}

long double dot(const Vector2& lhs, const Vector2& rhs) noexcept
{
    return std::fma(lhs.x, rhs.x, lhs.y * rhs.y);
}

long double dot(const Vector3& lhs, const Vector3& rhs) noexcept
{
    const auto xy = std::fma(lhs.x, rhs.x, lhs.y * rhs.y);
    return std::fma(lhs.z, rhs.z, xy);
}

long double norm(const Vector2& value) noexcept
{
    return std::hypotl(value.x, value.y);
}

long double norm(const Vector3& value) noexcept
{
    return std::sqrt(dot(value, value));
}

Vector3 normalize(const Vector3& value)
{
    const auto magnitude = norm(value);
    if (magnitude < kEpsilon)
    {
        throw std::invalid_argument("Cannot normalize the zero vector.");
    }

    return { value.x / magnitude, value.y / magnitude, value.z / magnitude };
}

std::string formatReal(long double value, const int precision)
{
    value = sanitizeZero(value);

    std::ostringstream stream;
    stream.setf(std::ios::fixed, std::ios::floatfield);
    stream << std::setprecision(precision) << value;

    auto text = stream.str();
    const auto dotPosition = text.find('.');
    if (dotPosition != std::string::npos)
    {
        while (!text.empty() && text.back() == '0')
        {
            text.pop_back();
        }

        if (!text.empty() && text.back() == '.')
        {
            text.pop_back();
        }
    }

    if (text == "-0")
    {
        return "0";
    }

    return text;
}

std::string formatComplex(const std::complex<long double>& value, const int precision)
{
    const auto real = sanitizeZero(value.real());
    const auto imag = sanitizeZero(value.imag());

    if (std::abs(imag) < kEpsilon)
    {
        return formatReal(real, precision);
    }

    std::ostringstream stream;
    if (std::abs(real) >= kEpsilon)
    {
        stream << formatReal(real, precision);
        stream << (imag < 0.0L ? " - " : " + ");
        stream << formatReal(std::abs(imag), precision) << "i";
    }
    else
    {
        if (imag < 0.0L)
        {
            stream << "-";
        }

        stream << formatReal(std::abs(imag), precision) << "i";
    }

    return stream.str();
}

std::string formatCoordinateTerm(const char variable, const long double offset)
{
    if (std::abs(offset) < kEpsilon)
    {
        return std::string(1, variable);
    }

    std::ostringstream stream;
    stream << variable << (offset >= 0.0L ? " - " : " + ") << formatReal(std::abs(offset));
    return stream.str();
}
} // namespace MathEngine

