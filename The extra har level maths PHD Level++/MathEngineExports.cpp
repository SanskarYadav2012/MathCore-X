#include "pch.h"

#include "MathEngineExports.h"

#include "AlgebraStats.h"
#include "AdvancedComputation.h"
#include "Geometry3D.h"
#include "SymbolicCalculus.h"

namespace
{
using MathEngine::kEpsilon;

MathEngineComplex64 toAbi(const std::complex<long double>& value)
{
    return { static_cast<double>(value.real()), static_cast<double>(value.imag()) };
}

int copyToWideBuffer(const std::string& text, wchar_t* buffer, const std::size_t bufferLength)
{
    const std::wstring wide(text.begin(), text.end());
    const auto requiredLength = static_cast<int>(wide.size() + 1U);

    if (buffer == nullptr || bufferLength == 0U)
    {
        return requiredLength;
    }

    const auto copyLength = std::min<std::size_t>(bufferLength - 1U, wide.size());
    std::copy_n(wide.c_str(), copyLength, buffer);
    buffer[copyLength] = L'\0';

    return requiredLength;
}

long double getParameter(const MathEngineFunctionSpec& spec, const std::size_t index, const long double defaultValue)
{
    return index < spec.parameterCount ? static_cast<long double>(spec.parameters[index]) : defaultValue;
}

MathEngine::SymbolicCalculus::LimitDirection toLimitDirection(const int direction)
{
    switch (direction)
    {
    case MathLimitLeft:
        return MathEngine::SymbolicCalculus::LimitDirection::Left;
    case MathLimitRight:
        return MathEngine::SymbolicCalculus::LimitDirection::Right;
    default:
        return MathEngine::SymbolicCalculus::LimitDirection::TwoSided;
    }
}

MathEngine::SymbolicCalculus::Expression buildExpression(const MathEngineFunctionSpec& spec)
{
    using MathEngine::SymbolicCalculus::Expression;
    using MathEngine::SymbolicCalculus::VariableMap;

    switch (spec.kind)
    {
    case MathFunctionPolynomial:
    {
        if (spec.parameterCount == 0U)
        {
            throw std::invalid_argument("Polynomial functions require at least one coefficient.");
        }

        std::vector<long double> coefficients;
        coefficients.reserve(spec.parameterCount);
        for (std::size_t index = 0; index < spec.parameterCount; ++index)
        {
            coefficients.push_back(static_cast<long double>(spec.parameters[index]));
        }

        std::ostringstream description;
        bool wroteTerm = false;
        for (std::size_t reverseIndex = coefficients.size(); reverseIndex > 0U; --reverseIndex)
        {
            const auto order = reverseIndex - 1U;
            const auto coefficient = coefficients[order];
            if (std::abs(coefficient) < kEpsilon)
            {
                continue;
            }

            if (wroteTerm)
            {
                description << (coefficient >= 0.0L ? " + " : " - ");
            }
            else if (coefficient < 0.0L)
            {
                description << "-";
            }

            description << MathEngine::formatReal(std::abs(coefficient));
            if (order > 0U)
            {
                description << "x";
                if (order > 1U)
                {
                    description << "^" << order;
                }
            }

            wroteTerm = true;
        }

        if (!wroteTerm)
        {
            description << "0";
        }

        return Expression(
            description.str(),
            [coefficients = std::move(coefficients)](const VariableMap& variables)
            {
                const auto x = static_cast<long double>(variables.at("x"));
                long double value = 0.0L;
                for (auto iterator = coefficients.rbegin(); iterator != coefficients.rend(); ++iterator)
                {
                    value = std::fma(value, x, *iterator);
                }

                return value;
            });
    }
    case MathFunctionSine:
    {
        const auto amplitude = getParameter(spec, 0U, 1.0L);
        const auto frequency = getParameter(spec, 1U, 1.0L);
        const auto phase = getParameter(spec, 2U, 0.0L);
        const auto vertical = getParameter(spec, 3U, 0.0L);

        return Expression(
            MathEngine::formatReal(amplitude) + "sin(" + MathEngine::formatReal(frequency) + "x + " +
                MathEngine::formatReal(phase) + ") + " + MathEngine::formatReal(vertical),
            [amplitude, frequency, phase, vertical](const VariableMap& variables)
            {
                const auto x = static_cast<long double>(variables.at("x"));
                return amplitude * std::sin(frequency * x + phase) + vertical;
            });
    }
    case MathFunctionCosine:
    {
        const auto amplitude = getParameter(spec, 0U, 1.0L);
        const auto frequency = getParameter(spec, 1U, 1.0L);
        const auto phase = getParameter(spec, 2U, 0.0L);
        const auto vertical = getParameter(spec, 3U, 0.0L);

        return Expression(
            MathEngine::formatReal(amplitude) + "cos(" + MathEngine::formatReal(frequency) + "x + " +
                MathEngine::formatReal(phase) + ") + " + MathEngine::formatReal(vertical),
            [amplitude, frequency, phase, vertical](const VariableMap& variables)
            {
                const auto x = static_cast<long double>(variables.at("x"));
                return amplitude * std::cos(frequency * x + phase) + vertical;
            });
    }
    case MathFunctionExponential:
    {
        const auto amplitude = getParameter(spec, 0U, 1.0L);
        const auto rate = getParameter(spec, 1U, 1.0L);
        const auto phase = getParameter(spec, 2U, 0.0L);
        const auto vertical = getParameter(spec, 3U, 0.0L);

        return Expression(
            MathEngine::formatReal(amplitude) + "exp(" + MathEngine::formatReal(rate) + "x + " +
                MathEngine::formatReal(phase) + ") + " + MathEngine::formatReal(vertical),
            [amplitude, rate, phase, vertical](const VariableMap& variables)
            {
                const auto x = static_cast<long double>(variables.at("x"));
                return amplitude * std::expl(rate * x + phase) + vertical;
            });
    }
    case MathFunctionLogarithm:
    {
        const auto amplitude = getParameter(spec, 0U, 1.0L);
        const auto scale = getParameter(spec, 1U, 1.0L);
        const auto shift = getParameter(spec, 2U, 0.0L);
        const auto vertical = getParameter(spec, 3U, 0.0L);

        return Expression(
            MathEngine::formatReal(amplitude) + "ln(" + MathEngine::formatReal(scale) + "x + " +
                MathEngine::formatReal(shift) + ") + " + MathEngine::formatReal(vertical),
            [amplitude, scale, shift, vertical](const VariableMap& variables)
            {
                const auto x = static_cast<long double>(variables.at("x"));
                const auto argument = scale * x + shift;
                if (argument <= 0.0L)
                {
                    throw std::domain_error("Logarithm argument must be positive.");
                }

                return amplitude * std::log(argument) + vertical;
            });
    }
    case MathFunctionReciprocal:
    {
        const auto amplitude = getParameter(spec, 0U, 1.0L);
        const auto scale = getParameter(spec, 1U, 1.0L);
        const auto shift = getParameter(spec, 2U, 0.0L);
        const auto vertical = getParameter(spec, 3U, 0.0L);

        return Expression(
            MathEngine::formatReal(amplitude) + " / (" + MathEngine::formatReal(scale) + "x + " +
                MathEngine::formatReal(shift) + ") + " + MathEngine::formatReal(vertical),
            [amplitude, scale, shift, vertical](const VariableMap& variables)
            {
                const auto x = static_cast<long double>(variables.at("x"));
                const auto denominator = scale * x + shift;
                if (std::abs(denominator) < kEpsilon)
                {
                    throw std::domain_error("Reciprocal denominator must be non-zero.");
                }

                return amplitude / denominator + vertical;
            });
    }
    case MathFunctionPower:
    {
        const auto amplitude = getParameter(spec, 0U, 1.0L);
        const auto scale = getParameter(spec, 1U, 1.0L);
        const auto shift = getParameter(spec, 2U, 0.0L);
        const auto exponent = getParameter(spec, 3U, 2.0L);
        const auto vertical = getParameter(spec, 4U, 0.0L);

        return Expression(
            MathEngine::formatReal(amplitude) + " * (" + MathEngine::formatReal(scale) + "x + " +
                MathEngine::formatReal(shift) + ")^" + MathEngine::formatReal(exponent) + " + " + MathEngine::formatReal(vertical),
            [amplitude, scale, shift, exponent, vertical](const VariableMap& variables)
            {
                const auto x = static_cast<long double>(variables.at("x"));
                return amplitude * std::pow(scale * x + shift, exponent) + vertical;
            });
    }
    case MathFunctionGaussian:
    {
        const auto amplitude = getParameter(spec, 0U, 1.0L);
        const auto mean = getParameter(spec, 1U, 0.0L);
        const auto sigma = getParameter(spec, 2U, 1.0L);
        const auto vertical = getParameter(spec, 3U, 0.0L);
        if (sigma <= 0.0L)
        {
            throw std::invalid_argument("Gaussian sigma must be positive.");
        }

        return Expression(
            MathEngine::formatReal(amplitude) + "exp(-((x - " + MathEngine::formatReal(mean) + ")^2)/(2*" +
                MathEngine::formatReal(sigma * sigma) + ")) + " + MathEngine::formatReal(vertical),
            [amplitude, mean, sigma, vertical](const VariableMap& variables)
            {
                const auto x = static_cast<long double>(variables.at("x"));
                const auto normalized = (x - mean) / sigma;
                return amplitude * std::exp(-0.5L * normalized * normalized) + vertical;
            });
    }
    default:
        throw std::invalid_argument("Unsupported function kind.");
    }
}

double performCalculusOperation(const MathEngineFunctionSpec* spec,
    const std::function<long double(const MathEngine::SymbolicCalculus::CalculusEngine&,
        const MathEngine::SymbolicCalculus::Expression&)>& operation)
{
    if (spec == nullptr)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }

    try
    {
        const auto expression = buildExpression(*spec);
        const MathEngine::SymbolicCalculus::CalculusEngine engine;
        return static_cast<double>(operation(engine, expression));
    }
    catch (...)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
}
} // namespace

MathEngineQuadraticRoots __cdecl SolveQuadraticExport(const double a, const double b, const double c)
{
    MathEngineQuadraticRoots abiResult {};
    try
    {
        const auto result = MathEngine::AlgebraStats::solveQuadratic(a, b, c);
        abiResult.count = (std::min)(result.roots.size(), static_cast<std::size_t>(2U));

        for (std::size_t index = 0; index < abiResult.count; ++index)
        {
            abiResult.roots[index] = toAbi(result.roots[index]);
        }
    }
    catch (...)
    {
        abiResult.count = 0U;
    }

    return abiResult;
}

MathEngineCubicRoots __cdecl SolveCubicExport(const double a, const double b, const double c, const double d)
{
    MathEngineCubicRoots abiResult {};
    try
    {
        const auto result = MathEngine::AlgebraStats::solveCubic(a, b, c, d);
        abiResult.count = (std::min)(result.roots.size(), static_cast<std::size_t>(3U));

        for (std::size_t index = 0; index < abiResult.count; ++index)
        {
            abiResult.roots[index] = toAbi(result.roots[index]);
        }
    }
    catch (...)
    {
        abiResult.count = 0U;
    }

    return abiResult;
}

double __cdecl MeanExport(const double* values, const std::size_t count)
{
    try
    {
        if (values == nullptr || count == 0U)
        {
            return std::numeric_limits<double>::quiet_NaN();
        }

        return static_cast<double>(MathEngine::AlgebraStats::mean(std::vector<double>(values, values + count)));
    }
    catch (...)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
}

double __cdecl MedianExport(const double* values, const std::size_t count)
{
    try
    {
        if (values == nullptr || count == 0U)
        {
            return std::numeric_limits<double>::quiet_NaN();
        }

        return static_cast<double>(MathEngine::AlgebraStats::median(std::vector<double>(values, values + count)));
    }
    catch (...)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
}

double __cdecl BayesExport(const double probabilityBGivenA, const double probabilityA, const double probabilityB)
{
    try
    {
        return static_cast<double>(MathEngine::AlgebraStats::Probability::bayes(probabilityBGivenA, probabilityA, probabilityB));
    }
    catch (...)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
}

double __cdecl ConditionalProbabilityExport(const double intersectionAB, const double probabilityB)
{
    try
    {
        return static_cast<double>(MathEngine::AlgebraStats::Probability::conditional(intersectionAB, probabilityB));
    }
    catch (...)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
}

double __cdecl ComputePiExport(const std::size_t iterations)
{
    try
    {
        return static_cast<double>(MathEngine::AdvancedComputation::computePi(iterations));
    }
    catch (...)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
}

int __cdecl GetCircleEquationExport(const double centerX, const double centerY, const double radius,
    wchar_t* buffer, const std::size_t bufferLength)
{
    try
    {
        const auto circle = MathEngine::Geometry3D::Circle({ centerX, centerY }, radius);
        return copyToWideBuffer(circle.equation(), buffer, bufferLength);
    }
    catch (...)
    {
        if (buffer != nullptr && bufferLength > 0U)
        {
            buffer[0] = L'\0';
        }

        return -1;
    }
}

int __cdecl GetPlaneEquationExport(const double a, const double b, const double c, const double d,
    wchar_t* buffer, const std::size_t bufferLength)
{
    try
    {
        const auto plane = MathEngine::Geometry3D::Plane(a, b, c, d);
        return copyToWideBuffer(plane.equation(), buffer, bufferLength);
    }
    catch (...)
    {
        if (buffer != nullptr && bufferLength > 0U)
        {
            buffer[0] = L'\0';
        }

        return -1;
    }
}

double __cdecl EvaluateFunctionExport(const MathEngineFunctionSpec* spec, const double x)
{
    if (spec == nullptr)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }

    try
    {
        const auto expression = buildExpression(*spec);
        return static_cast<double>(expression.evaluate({ { "x", static_cast<long double>(x) } }));
    }
    catch (...)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
}

double __cdecl DifferentiateFunctionExport(const MathEngineFunctionSpec* spec, const double at)
{
    return performCalculusOperation(
        spec,
        [at](const MathEngine::SymbolicCalculus::CalculusEngine& engine,
            const MathEngine::SymbolicCalculus::Expression& expression)
        {
            return engine.derivative(expression, "x", at);
        });
}

double __cdecl SecondDerivativeFunctionExport(const MathEngineFunctionSpec* spec, const double at)
{
    return performCalculusOperation(
        spec,
        [at](const MathEngine::SymbolicCalculus::CalculusEngine& engine,
            const MathEngine::SymbolicCalculus::Expression& expression)
        {
            return engine.secondDerivative(expression, "x", at);
        });
}

double __cdecl IntegrateFunctionExport(const MathEngineFunctionSpec* spec, const double lower, const double upper)
{
    return performCalculusOperation(
        spec,
        [lower, upper](const MathEngine::SymbolicCalculus::CalculusEngine& engine,
            const MathEngine::SymbolicCalculus::Expression& expression)
        {
            return engine.integrateDefinite(expression, "x", lower, upper);
        });
}

double __cdecl LimitFunctionExport(const MathEngineFunctionSpec* spec, const double point, const int direction)
{
    return performCalculusOperation(
        spec,
        [point, direction](const MathEngine::SymbolicCalculus::CalculusEngine& engine,
            const MathEngine::SymbolicCalculus::Expression& expression)
        {
            return engine.limit(expression, "x", point, toLimitDirection(direction));
        });
}

int __cdecl DescribeFunctionExport(const MathEngineFunctionSpec* spec, wchar_t* buffer, const std::size_t bufferLength)
{
    if (spec == nullptr)
    {
        if (buffer != nullptr && bufferLength > 0U)
        {
            buffer[0] = L'\0';
        }

        return -1;
    }

    try
    {
        return copyToWideBuffer(buildExpression(*spec).description(), buffer, bufferLength);
    }
    catch (...)
    {
        if (buffer != nullptr && bufferLength > 0U)
        {
            buffer[0] = L'\0';
        }

        return -1;
    }
}

double __cdecl MaclaurinExpExport(const double x, const std::size_t terms)
{
    try
    {
        return static_cast<double>(MathEngine::AdvancedComputation::maclaurinExp(x, terms));
    }
    catch (...)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
}

double __cdecl MaclaurinSinExport(const double x, const std::size_t terms)
{
    try
    {
        return static_cast<double>(MathEngine::AdvancedComputation::maclaurinSin(x, terms));
    }
    catch (...)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
}

double __cdecl MaclaurinCosExport(const double x, const std::size_t terms)
{
    try
    {
        return static_cast<double>(MathEngine::AdvancedComputation::maclaurinCos(x, terms));
    }
    catch (...)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
}

double __cdecl DominantEigenvalueExport(const double* matrixValues, const std::size_t dimension)
{
    try
    {
        if (matrixValues == nullptr || dimension == 0U)
        {
            return std::numeric_limits<double>::quiet_NaN();
        }

        MathEngine::AdvancedComputation::Matrix matrix(dimension, dimension, 0.0L);
        for (std::size_t row = 0U; row < dimension; ++row)
        {
            for (std::size_t column = 0U; column < dimension; ++column)
            {
                matrix(row, column) = matrixValues[row * dimension + column];
            }
        }

        return static_cast<double>(MathEngine::AdvancedComputation::powerIteration(matrix).eigenvalue);
    }
    catch (...)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
}

double __cdecl ComputeGoldenRatioExport(const std::size_t iterations)
{
    try
    {
        return static_cast<double>(MathEngine::AdvancedComputation::computeGoldenRatio(iterations));
    }
    catch (...)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
}
