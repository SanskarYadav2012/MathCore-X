#include "pch.h"

#include "Geometry3D.h"

namespace MathEngine::Geometry3D
{
namespace
{
void ensurePositive(const long double value, const std::string& name)
{
    if (value <= 0.0L)
    {
        throw std::invalid_argument(name + " must be strictly positive.");
    }
}

std::string squaredTerm(const char variable, const long double offset)
{
    return "(" + formatCoordinateTerm(variable, offset) + ")^2";
}

std::string denominatorTerm(const char variable, const long double offset, const long double denominator)
{
    return squaredTerm(variable, offset) + " / " + formatReal(square(denominator));
}
} // namespace

StraightLine::StraightLine(const long double a, const long double b, const long double c)
    : a_(a), b_(b), c_(c)
{
    const auto length = std::hypotl(a_, b_);
    if (length < kEpsilon)
    {
        throw std::invalid_argument("A straight line requires at least one non-zero directional coefficient.");
    }

    // Normalizing makes the same geometric line compare and print consistently.
    a_ /= length;
    b_ /= length;
    c_ /= length;
}

StraightLine StraightLine::fromSlopeIntercept(const long double slope, const long double intercept)
{
    // y = mx + b is rearranged to mx - y + b = 0.
    return StraightLine(slope, -1.0L, intercept);
}

StraightLine StraightLine::fromPoints(const Vector2& first, const Vector2& second)
{
    if (norm(Vector2 { second.x - first.x, second.y - first.y }) < kEpsilon)
    {
        throw std::invalid_argument("Two distinct points are required to define a line.");
    }

    // Using the determinant form gives a stable implicit equation.
    const auto a = first.y - second.y;
    const auto b = second.x - first.x;
    const auto c = std::fma(first.x, second.y, -second.x * first.y);
    return StraightLine(a, b, c);
}

std::string StraightLine::equation() const
{
    std::ostringstream stream;
    stream << formatReal(a_) << "x ";
    stream << (b_ < 0.0L ? "- " : "+ ") << formatReal(std::abs(b_)) << "y ";
    stream << (c_ < 0.0L ? "- " : "+ ") << formatReal(std::abs(c_)) << " = 0";
    return stream.str();
}

Circle::Circle(Vector2 center, const long double radius)
    : center_(center), radius_(radius)
{
    ensurePositive(radius_, "Circle radius");
}

std::string Circle::equation() const
{
    std::ostringstream stream;
    stream << squaredTerm('x', center_.x) << " + " << squaredTerm('y', center_.y)
           << " = " << formatReal(square(radius_));
    return stream.str();
}

Parabola::Parabola(Vector2 vertex, const long double focalParameter, const Axis2D axis)
    : vertex_(vertex), focalParameter_(focalParameter), axis_(axis)
{
    if (std::abs(focalParameter_) < kEpsilon)
    {
        throw std::invalid_argument("Parabola focal parameter must be non-zero.");
    }
}

Vector2 Parabola::focus() const noexcept
{
    if (axis_ == Axis2D::Vertical)
    {
        return { vertex_.x, vertex_.y + focalParameter_ };
    }

    return { vertex_.x + focalParameter_, vertex_.y };
}

std::string Parabola::equation() const
{
    std::ostringstream stream;
    if (axis_ == Axis2D::Vertical)
    {
        // (x - h)^2 = 4p(y - k), where p measures vertex-to-focus distance.
        stream << squaredTerm('x', vertex_.x) << " = " << formatReal(4.0L * focalParameter_)
               << "(" << formatCoordinateTerm('y', vertex_.y) << ")";
    }
    else
    {
        stream << squaredTerm('y', vertex_.y) << " = " << formatReal(4.0L * focalParameter_)
               << "(" << formatCoordinateTerm('x', vertex_.x) << ")";
    }

    return stream.str();
}

Ellipse::Ellipse(Vector2 center, const long double semiMajorAxis, const long double semiMinorAxis, const Axis2D majorAxis)
    : center_(center), semiMajorAxis_(semiMajorAxis), semiMinorAxis_(semiMinorAxis), majorAxis_(majorAxis)
{
    ensurePositive(semiMajorAxis_, "Ellipse semi-major axis");
    ensurePositive(semiMinorAxis_, "Ellipse semi-minor axis");

    if (semiMajorAxis_ < semiMinorAxis_)
    {
        throw std::invalid_argument("Ellipse semi-major axis must be at least as large as the semi-minor axis.");
    }
}

long double Ellipse::focalDistance() const noexcept
{
    // For an ellipse, c^2 = a^2 - b^2.
    return std::sqrt((std::max)(0.0L, square(semiMajorAxis_) - square(semiMinorAxis_)));
}

std::pair<Vector2, Vector2> Ellipse::foci() const noexcept
{
    const auto c = focalDistance();
    if (majorAxis_ == Axis2D::Horizontal)
    {
        return { { center_.x - c, center_.y }, { center_.x + c, center_.y } };
    }

    return { { center_.x, center_.y - c }, { center_.x, center_.y + c } };
}

std::string Ellipse::equation() const
{
    std::ostringstream stream;
    if (majorAxis_ == Axis2D::Horizontal)
    {
        stream << denominatorTerm('x', center_.x, semiMajorAxis_) << " + "
               << denominatorTerm('y', center_.y, semiMinorAxis_) << " = 1";
    }
    else
    {
        stream << denominatorTerm('x', center_.x, semiMinorAxis_) << " + "
               << denominatorTerm('y', center_.y, semiMajorAxis_) << " = 1";
    }

    return stream.str();
}

Hyperbola::Hyperbola(Vector2 center, const long double transverseAxis, const long double conjugateAxis,
    const Axis2D transverseDirection)
    : center_(center), transverseAxis_(transverseAxis), conjugateAxis_(conjugateAxis), transverseDirection_(transverseDirection)
{
    ensurePositive(transverseAxis_, "Hyperbola transverse axis");
    ensurePositive(conjugateAxis_, "Hyperbola conjugate axis");
}

long double Hyperbola::focalDistance() const noexcept
{
    // For a hyperbola, c^2 = a^2 + b^2.
    return std::sqrt(square(transverseAxis_) + square(conjugateAxis_));
}

std::pair<Vector2, Vector2> Hyperbola::foci() const noexcept
{
    const auto c = focalDistance();
    if (transverseDirection_ == Axis2D::Horizontal)
    {
        return { { center_.x - c, center_.y }, { center_.x + c, center_.y } };
    }

    return { { center_.x, center_.y - c }, { center_.x, center_.y + c } };
}

std::string Hyperbola::equation() const
{
    std::ostringstream stream;
    if (transverseDirection_ == Axis2D::Horizontal)
    {
        stream << denominatorTerm('x', center_.x, transverseAxis_) << " - "
               << denominatorTerm('y', center_.y, conjugateAxis_) << " = 1";
    }
    else
    {
        stream << denominatorTerm('y', center_.y, transverseAxis_) << " - "
               << denominatorTerm('x', center_.x, conjugateAxis_) << " = 1";
    }

    return stream.str();
}

Plane::Plane(const long double a, const long double b, const long double c, const long double d)
    : normal_(normalize({ a, b, c })), d_(d)
{
    const auto length = std::sqrt(square(a) + square(b) + square(c));
    d_ /= length;
}

Plane::Plane(const Vector3 point, const Vector3 normal)
    : normal_(normalize(normal))
{
    // A plane is n . x + d = 0. Substituting a known point solves d.
    d_ = -dot(normal_, point);
}

std::string Plane::equation() const
{
    std::ostringstream stream;
    stream << formatReal(normal_.x) << "x "
           << (normal_.y < 0.0L ? "- " : "+ ") << formatReal(std::abs(normal_.y)) << "y "
           << (normal_.z < 0.0L ? "- " : "+ ") << formatReal(std::abs(normal_.z)) << "z "
           << (d_ < 0.0L ? "- " : "+ ") << formatReal(std::abs(d_)) << " = 0";
    return stream.str();
}

Sphere::Sphere(Vector3 center, const long double radius)
    : center_(center), radius_(radius)
{
    ensurePositive(radius_, "Sphere radius");
}

std::string Sphere::equation() const
{
    std::ostringstream stream;
    stream << squaredTerm('x', center_.x) << " + " << squaredTerm('y', center_.y) << " + "
           << squaredTerm('z', center_.z) << " = " << formatReal(square(radius_));
    return stream.str();
}

Ellipsoid::Ellipsoid(Vector3 center, const long double axisX, const long double axisY, const long double axisZ)
    : center_(center), axisX_(axisX), axisY_(axisY), axisZ_(axisZ)
{
    ensurePositive(axisX_, "Ellipsoid x-axis");
    ensurePositive(axisY_, "Ellipsoid y-axis");
    ensurePositive(axisZ_, "Ellipsoid z-axis");
}

std::array<long double, 3> Ellipsoid::semiAxes() const noexcept
{
    return { axisX_, axisY_, axisZ_ };
}

std::string Ellipsoid::equation() const
{
    std::ostringstream stream;
    stream << denominatorTerm('x', center_.x, axisX_) << " + "
           << denominatorTerm('y', center_.y, axisY_) << " + "
           << denominatorTerm('z', center_.z, axisZ_) << " = 1";
    return stream.str();
}

Paraboloid::Paraboloid(Vector3 vertex, const long double scaleX, const long double scaleY,
    const long double focalParameter, const Axis3D axis)
    : vertex_(vertex), scaleX_(scaleX), scaleY_(scaleY), focalParameter_(focalParameter), axis_(axis)
{
    ensurePositive(scaleX_, "Paraboloid scaleX");
    ensurePositive(scaleY_, "Paraboloid scaleY");
    if (std::abs(focalParameter_) < kEpsilon)
    {
        throw std::invalid_argument("Paraboloid focal parameter must be non-zero.");
    }
}

std::string Paraboloid::equation() const
{
    std::ostringstream stream;
    if (axis_ == Axis3D::Z)
    {
        stream << denominatorTerm('x', vertex_.x, scaleX_) << " + "
               << denominatorTerm('y', vertex_.y, scaleY_) << " = "
               << formatReal(4.0L * focalParameter_) << "(" << formatCoordinateTerm('z', vertex_.z) << ")";
    }
    else if (axis_ == Axis3D::Y)
    {
        stream << denominatorTerm('x', vertex_.x, scaleX_) << " + "
               << denominatorTerm('z', vertex_.z, scaleY_) << " = "
               << formatReal(4.0L * focalParameter_) << "(" << formatCoordinateTerm('y', vertex_.y) << ")";
    }
    else
    {
        stream << denominatorTerm('y', vertex_.y, scaleX_) << " + "
               << denominatorTerm('z', vertex_.z, scaleY_) << " = "
               << formatReal(4.0L * focalParameter_) << "(" << formatCoordinateTerm('x', vertex_.x) << ")";
    }

    return stream.str();
}

Hyperboloid::Hyperboloid(Vector3 center, const long double axisA, const long double axisB, const long double axisC,
    const Axis3D principalAxis, const HyperboloidType type)
    : center_(center), axisA_(axisA), axisB_(axisB), axisC_(axisC), principalAxis_(principalAxis), type_(type)
{
    ensurePositive(axisA_, "Hyperboloid axisA");
    ensurePositive(axisB_, "Hyperboloid axisB");
    ensurePositive(axisC_, "Hyperboloid axisC");
}

std::string Hyperboloid::equation() const
{
    const auto oneSheet = type_ == HyperboloidType::OneSheet;
    const auto positiveAxis = [oneSheet](const bool isPrincipalAxis) noexcept
    {
        return oneSheet ? !isPrincipalAxis : isPrincipalAxis;
    };

    const auto isXPrincipal = principalAxis_ == Axis3D::X;
    const auto isYPrincipal = principalAxis_ == Axis3D::Y;
    const auto isZPrincipal = principalAxis_ == Axis3D::Z;

    const std::array<std::pair<bool, std::string>, 3> terms {
        std::pair { positiveAxis(isXPrincipal), denominatorTerm('x', center_.x, axisA_) },
        std::pair { positiveAxis(isYPrincipal), denominatorTerm('y', center_.y, axisB_) },
        std::pair { positiveAxis(isZPrincipal), denominatorTerm('z', center_.z, axisC_) }
    };

    std::ostringstream stream;
    for (std::size_t index = 0; index < terms.size(); ++index)
    {
        const auto& [positive, text] = terms[index];
        if (index == 0U)
        {
            stream << (positive ? "" : "-") << text;
        }
        else
        {
            stream << (positive ? " + " : " - ") << text;
        }
    }

    stream << " = 1";
    return stream.str();
}
} // namespace MathEngine::Geometry3D
