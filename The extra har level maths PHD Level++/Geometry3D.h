#pragma once

#include "MathCore.h"

namespace MathEngine::Geometry3D
{
enum class Axis2D
{
    Horizontal,
    Vertical
};

enum class Axis3D
{
    X,
    Y,
    Z
};

enum class HyperboloidType
{
    OneSheet,
    TwoSheets
};

class StraightLine
{
public:
    StraightLine(long double a, long double b, long double c);

    [[nodiscard]] static StraightLine fromSlopeIntercept(long double slope, long double intercept);
    [[nodiscard]] static StraightLine fromPoints(const Vector2& first, const Vector2& second);

    [[nodiscard]] long double a() const noexcept { return a_; }
    [[nodiscard]] long double b() const noexcept { return b_; }
    [[nodiscard]] long double c() const noexcept { return c_; }
    [[nodiscard]] std::string equation() const;

private:
    long double a_ {};
    long double b_ {};
    long double c_ {};
};

class Circle
{
public:
    Circle(Vector2 center, long double radius);

    [[nodiscard]] const Vector2& center() const noexcept { return center_; }
    [[nodiscard]] long double radius() const noexcept { return radius_; }
    [[nodiscard]] std::string equation() const;

private:
    Vector2 center_ {};
    long double radius_ {};
};

class Parabola
{
public:
    Parabola(Vector2 vertex, long double focalParameter, Axis2D axis = Axis2D::Vertical);

    [[nodiscard]] const Vector2& vertex() const noexcept { return vertex_; }
    [[nodiscard]] long double focalParameter() const noexcept { return focalParameter_; }
    [[nodiscard]] Axis2D axis() const noexcept { return axis_; }
    [[nodiscard]] Vector2 focus() const noexcept;
    [[nodiscard]] std::string equation() const;

private:
    Vector2 vertex_ {};
    long double focalParameter_ {};
    Axis2D axis_ { Axis2D::Vertical };
};

class Ellipse
{
public:
    Ellipse(Vector2 center, long double semiMajorAxis, long double semiMinorAxis, Axis2D majorAxis = Axis2D::Horizontal);

    [[nodiscard]] const Vector2& center() const noexcept { return center_; }
    [[nodiscard]] long double semiMajorAxis() const noexcept { return semiMajorAxis_; }
    [[nodiscard]] long double semiMinorAxis() const noexcept { return semiMinorAxis_; }
    [[nodiscard]] Axis2D majorAxis() const noexcept { return majorAxis_; }
    [[nodiscard]] long double focalDistance() const noexcept;
    [[nodiscard]] std::pair<Vector2, Vector2> foci() const noexcept;
    [[nodiscard]] std::string equation() const;

private:
    Vector2 center_ {};
    long double semiMajorAxis_ {};
    long double semiMinorAxis_ {};
    Axis2D majorAxis_ { Axis2D::Horizontal };
};

class Hyperbola
{
public:
    Hyperbola(Vector2 center, long double transverseAxis, long double conjugateAxis, Axis2D transverseDirection = Axis2D::Horizontal);

    [[nodiscard]] const Vector2& center() const noexcept { return center_; }
    [[nodiscard]] long double transverseAxis() const noexcept { return transverseAxis_; }
    [[nodiscard]] long double conjugateAxis() const noexcept { return conjugateAxis_; }
    [[nodiscard]] Axis2D transverseDirection() const noexcept { return transverseDirection_; }
    [[nodiscard]] long double focalDistance() const noexcept;
    [[nodiscard]] std::pair<Vector2, Vector2> foci() const noexcept;
    [[nodiscard]] std::string equation() const;

private:
    Vector2 center_ {};
    long double transverseAxis_ {};
    long double conjugateAxis_ {};
    Axis2D transverseDirection_ { Axis2D::Horizontal };
};

class Plane
{
public:
    Plane(long double a, long double b, long double c, long double d);
    Plane(Vector3 point, Vector3 normal);

    [[nodiscard]] const Vector3& normal() const noexcept { return normal_; }
    [[nodiscard]] long double d() const noexcept { return d_; }
    [[nodiscard]] std::string equation() const;

private:
    Vector3 normal_ {};
    long double d_ {};
};

class Sphere
{
public:
    Sphere(Vector3 center, long double radius);

    [[nodiscard]] const Vector3& center() const noexcept { return center_; }
    [[nodiscard]] long double radius() const noexcept { return radius_; }
    [[nodiscard]] std::string equation() const;

private:
    Vector3 center_ {};
    long double radius_ {};
};

class Ellipsoid
{
public:
    Ellipsoid(Vector3 center, long double axisX, long double axisY, long double axisZ);

    [[nodiscard]] const Vector3& center() const noexcept { return center_; }
    [[nodiscard]] std::array<long double, 3> semiAxes() const noexcept;
    [[nodiscard]] std::string equation() const;

private:
    Vector3 center_ {};
    long double axisX_ {};
    long double axisY_ {};
    long double axisZ_ {};
};

class Paraboloid
{
public:
    Paraboloid(Vector3 vertex, long double scaleX, long double scaleY, long double focalParameter, Axis3D axis = Axis3D::Z);

    [[nodiscard]] const Vector3& vertex() const noexcept { return vertex_; }
    [[nodiscard]] Axis3D axis() const noexcept { return axis_; }
    [[nodiscard]] std::string equation() const;

private:
    Vector3 vertex_ {};
    long double scaleX_ {};
    long double scaleY_ {};
    long double focalParameter_ {};
    Axis3D axis_ { Axis3D::Z };
};

class Hyperboloid
{
public:
    Hyperboloid(Vector3 center, long double axisA, long double axisB, long double axisC,
        Axis3D principalAxis = Axis3D::Z, HyperboloidType type = HyperboloidType::OneSheet);

    [[nodiscard]] const Vector3& center() const noexcept { return center_; }
    [[nodiscard]] Axis3D principalAxis() const noexcept { return principalAxis_; }
    [[nodiscard]] HyperboloidType type() const noexcept { return type_; }
    [[nodiscard]] std::string equation() const;

private:
    Vector3 center_ {};
    long double axisA_ {};
    long double axisB_ {};
    long double axisC_ {};
    Axis3D principalAxis_ { Axis3D::Z };
    HyperboloidType type_ { HyperboloidType::OneSheet };
};
} // namespace MathEngine::Geometry3D

