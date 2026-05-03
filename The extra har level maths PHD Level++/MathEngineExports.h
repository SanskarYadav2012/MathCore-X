#pragma once

#include <cstddef>

#ifdef THEEXTRAHARLEVELMATHSPHDLEVEL_EXPORTS
#define MATHENGINE_API __declspec(dllexport)
#else
#define MATHENGINE_API __declspec(dllimport)
#endif

#define MATHENGINE_MAX_FUNCTION_PARAMETERS 16

extern "C"
{
enum MathEngineFunctionKind
{
    MathFunctionPolynomial = 1,
    MathFunctionSine = 2,
    MathFunctionCosine = 3,
    MathFunctionExponential = 4,
    MathFunctionLogarithm = 5,
    MathFunctionReciprocal = 6,
    MathFunctionPower = 7,
    MathFunctionGaussian = 8
};

enum MathEngineLimitDirection
{
    MathLimitLeft = -1,
    MathLimitTwoSided = 0,
    MathLimitRight = 1
};

struct MathEngineComplex64
{
    double real;
    double imaginary;
};

struct MathEngineQuadraticRoots
{
    MathEngineComplex64 roots[2];
    std::size_t count;
};

struct MathEngineCubicRoots
{
    MathEngineComplex64 roots[3];
    std::size_t count;
};

struct MathEngineFunctionSpec
{
    int kind;
    double parameters[MATHENGINE_MAX_FUNCTION_PARAMETERS];
    std::size_t parameterCount;
};

MATHENGINE_API MathEngineQuadraticRoots __cdecl SolveQuadraticExport(double a, double b, double c);
MATHENGINE_API MathEngineCubicRoots __cdecl SolveCubicExport(double a, double b, double c, double d);

MATHENGINE_API double __cdecl MeanExport(const double* values, std::size_t count);
MATHENGINE_API double __cdecl MedianExport(const double* values, std::size_t count);
MATHENGINE_API double __cdecl BayesExport(double probabilityBGivenA, double probabilityA, double probabilityB);
MATHENGINE_API double __cdecl ConditionalProbabilityExport(double intersectionAB, double probabilityB);
MATHENGINE_API double __cdecl ComputePiExport(std::size_t iterations);

MATHENGINE_API int __cdecl GetCircleEquationExport(double centerX, double centerY, double radius,
    wchar_t* buffer, std::size_t bufferLength);

MATHENGINE_API int __cdecl GetPlaneEquationExport(double a, double b, double c, double d,
    wchar_t* buffer, std::size_t bufferLength);

MATHENGINE_API double __cdecl EvaluateFunctionExport(const MathEngineFunctionSpec* spec, double x);
MATHENGINE_API double __cdecl DifferentiateFunctionExport(const MathEngineFunctionSpec* spec, double at);
MATHENGINE_API double __cdecl SecondDerivativeFunctionExport(const MathEngineFunctionSpec* spec, double at);
MATHENGINE_API double __cdecl IntegrateFunctionExport(const MathEngineFunctionSpec* spec, double lower, double upper);
MATHENGINE_API double __cdecl LimitFunctionExport(const MathEngineFunctionSpec* spec, double point, int direction);
MATHENGINE_API int __cdecl DescribeFunctionExport(const MathEngineFunctionSpec* spec, wchar_t* buffer, std::size_t bufferLength);

MATHENGINE_API double __cdecl MaclaurinExpExport(double x, std::size_t terms);
MATHENGINE_API double __cdecl MaclaurinSinExport(double x, std::size_t terms);
MATHENGINE_API double __cdecl MaclaurinCosExport(double x, std::size_t terms);
MATHENGINE_API double __cdecl DominantEigenvalueExport(const double* matrixValues, std::size_t dimension);
MATHENGINE_API double __cdecl ComputeGoldenRatioExport(std::size_t iterations);
}
