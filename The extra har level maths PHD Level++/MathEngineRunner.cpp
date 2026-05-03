#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "MathEngineExports.h"

namespace
{
void printMenu()
{
    std::cout << "\nMath Engine Runner\n";
    std::cout << "1. Solve quadratic equation\n";
    std::cout << "2. Solve cubic equation\n";
    std::cout << "3. Compute mean and median\n";
    std::cout << "4. Conditional probability and Bayes theorem\n";
    std::cout << "5. Show circle equation\n";
    std::cout << "6. Show plane equation\n";
    std::cout << "7. Approximate pi\n";
    std::cout << "8. Calculus tools (evaluate, derivative, integral, limit)\n";
    std::cout << "9. Maclaurin series approximations\n";
    std::cout << "10. Dominant eigenvalue of a matrix\n";
    std::cout << "11. Golden ratio continued fraction\n";
    std::cout << "0. Exit\n";
    std::cout << "Choose an option: ";
}

void clearInput()
{
    std::cin.clear();
    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
}

bool readDouble(const std::string& prompt, double& value)
{
    std::cout << prompt;
    if (!(std::cin >> value))
    {
        clearInput();
        std::cout << "Invalid numeric input.\n";
        return false;
    }

    return true;
}

bool readSize(const std::string& prompt, std::size_t& value)
{
    std::cout << prompt;
    if (!(std::cin >> value))
    {
        clearInput();
        std::cout << "Invalid integer input.\n";
        return false;
    }

    return true;
}

bool readInt(const std::string& prompt, int& value)
{
    std::cout << prompt;
    if (!(std::cin >> value))
    {
        clearInput();
        std::cout << "Invalid integer input.\n";
        return false;
    }

    return true;
}

void printComplex(const MathEngineComplex64& value)
{
    std::cout << value.real;
    if (value.imaginary > 0.0)
    {
        std::cout << " + " << value.imaginary << "i";
    }
    else if (value.imaginary < 0.0)
    {
        std::cout << " - " << std::abs(value.imaginary) << "i";
    }
}

bool describeFunction(const MathEngineFunctionSpec& spec)
{
    std::wstring buffer(512, L'\0');
    const auto result = DescribeFunctionExport(&spec, buffer.data(), buffer.size());
    if (result < 0)
    {
        std::cout << "Unable to describe the function.\n";
        return false;
    }

    std::wcout << L"Function: " << buffer.c_str() << L'\n';
    return true;
}

bool readFunctionSpec(MathEngineFunctionSpec& spec)
{
    spec = {};

    std::cout << "\nSelect function family\n";
    std::cout << "1. Polynomial: a0 + a1*x + a2*x^2 + ...\n";
    std::cout << "2. Sine: a*sin(bx + c) + d\n";
    std::cout << "3. Cosine: a*cos(bx + c) + d\n";
    std::cout << "4. Exponential: a*exp(bx + c) + d\n";
    std::cout << "5. Logarithm: a*ln(bx + c) + d\n";
    std::cout << "6. Reciprocal: a/(bx + c) + d\n";
    std::cout << "7. Power: a*(bx + c)^n + d\n";
    std::cout << "8. Gaussian: a*exp(-((x-mu)^2)/(2*sigma^2)) + d\n";

    int choice = 0;
    if (!readInt("Choose function type: ", choice))
    {
        return false;
    }

    switch (choice)
    {
    case MathFunctionPolynomial:
    {
        std::size_t degree = 0U;
        if (!readSize("Polynomial degree: ", degree))
        {
            return false;
        }

        if (degree + 1U > MATHENGINE_MAX_FUNCTION_PARAMETERS)
        {
            std::cout << "Maximum supported degree is " << (MATHENGINE_MAX_FUNCTION_PARAMETERS - 1U) << ".\n";
            return false;
        }

        spec.kind = MathFunctionPolynomial;
        spec.parameterCount = degree + 1U;
        for (std::size_t index = 0U; index <= degree; ++index)
        {
            if (!readDouble("Coefficient a" + std::to_string(index) + ": ", spec.parameters[index]))
            {
                return false;
            }
        }

        return true;
    }
    case MathFunctionSine:
    case MathFunctionCosine:
    case MathFunctionExponential:
    case MathFunctionLogarithm:
    case MathFunctionReciprocal:
    {
        spec.kind = choice;
        spec.parameterCount = 4U;
        if (!readDouble("Amplitude a: ", spec.parameters[0]) ||
            !readDouble("Scale/rate b: ", spec.parameters[1]) ||
            !readDouble("Shift c: ", spec.parameters[2]) ||
            !readDouble("Vertical shift d: ", spec.parameters[3]))
        {
            return false;
        }

        return true;
    }
    case MathFunctionPower:
    {
        spec.kind = choice;
        spec.parameterCount = 5U;
        if (!readDouble("Amplitude a: ", spec.parameters[0]) ||
            !readDouble("Scale b: ", spec.parameters[1]) ||
            !readDouble("Shift c: ", spec.parameters[2]) ||
            !readDouble("Exponent n: ", spec.parameters[3]) ||
            !readDouble("Vertical shift d: ", spec.parameters[4]))
        {
            return false;
        }

        return true;
    }
    case MathFunctionGaussian:
    {
        spec.kind = choice;
        spec.parameterCount = 4U;
        if (!readDouble("Amplitude a: ", spec.parameters[0]) ||
            !readDouble("Mean mu: ", spec.parameters[1]) ||
            !readDouble("Standard deviation sigma: ", spec.parameters[2]) ||
            !readDouble("Vertical shift d: ", spec.parameters[3]))
        {
            return false;
        }

        return true;
    }
    default:
        std::cout << "Unknown function type.\n";
        return false;
    }
}

void solveQuadraticInteractive()
{
    double a = 0.0;
    double b = 0.0;
    double c = 0.0;
    if (!readDouble("Enter a: ", a) || !readDouble("Enter b: ", b) || !readDouble("Enter c: ", c))
    {
        return;
    }

    const auto result = SolveQuadraticExport(a, b, c);
    if (result.count == 0U)
    {
        std::cout << "Solver returned no roots.\n";
        return;
    }

    std::cout << "Roots:\n";
    for (std::size_t index = 0; index < result.count; ++index)
    {
        std::cout << "  r" << (index + 1U) << " = ";
        printComplex(result.roots[index]);
        std::cout << '\n';
    }
}

void solveCubicInteractive()
{
    double a = 0.0;
    double b = 0.0;
    double c = 0.0;
    double d = 0.0;
    if (!readDouble("Enter a: ", a) || !readDouble("Enter b: ", b) || !readDouble("Enter c: ", c) || !readDouble("Enter d: ", d))
    {
        return;
    }

    const auto result = SolveCubicExport(a, b, c, d);
    if (result.count == 0U)
    {
        std::cout << "Solver returned no roots.\n";
        return;
    }

    std::cout << "Roots:\n";
    for (std::size_t index = 0; index < result.count; ++index)
    {
        std::cout << "  r" << (index + 1U) << " = ";
        printComplex(result.roots[index]);
        std::cout << '\n';
    }
}

void statsInteractive()
{
    std::size_t count = 0U;
    if (!readSize("How many values? ", count) || count == 0U)
    {
        std::cout << "Count must be positive.\n";
        return;
    }

    std::vector<double> values(count, 0.0);
    for (std::size_t index = 0; index < count; ++index)
    {
        if (!readDouble("Value " + std::to_string(index + 1U) + ": ", values[index]))
        {
            return;
        }
    }

    const auto mean = MeanExport(values.data(), values.size());
    const auto median = MedianExport(values.data(), values.size());

    std::cout << "Mean   = " << mean << '\n';
    std::cout << "Median = " << median << '\n';
}

void probabilityInteractive()
{
    double pBGivenA = 0.0;
    double pA = 0.0;
    double pB = 0.0;
    double pIntersection = 0.0;

    if (!readDouble("Enter P(B|A): ", pBGivenA) ||
        !readDouble("Enter P(A): ", pA) ||
        !readDouble("Enter P(B): ", pB) ||
        !readDouble("Enter P(A intersect B): ", pIntersection))
    {
        return;
    }

    std::cout << "Bayes result P(A|B) = " << BayesExport(pBGivenA, pA, pB) << '\n';
    std::cout << "Conditional P(A|B)  = " << ConditionalProbabilityExport(pIntersection, pB) << '\n';
}

void circleEquationInteractive()
{
    double x = 0.0;
    double y = 0.0;
    double radius = 0.0;
    if (!readDouble("Center x: ", x) || !readDouble("Center y: ", y) || !readDouble("Radius: ", radius))
    {
        return;
    }

    std::wstring buffer(512, L'\0');
    const auto required = GetCircleEquationExport(x, y, radius, buffer.data(), buffer.size());
    if (required < 0)
    {
        std::cout << "Failed to build circle equation.\n";
        return;
    }

    std::wcout << L"Equation: " << buffer.c_str() << L'\n';
}

void planeEquationInteractive()
{
    double a = 0.0;
    double b = 0.0;
    double c = 0.0;
    double d = 0.0;
    if (!readDouble("Plane coefficient a: ", a) ||
        !readDouble("Plane coefficient b: ", b) ||
        !readDouble("Plane coefficient c: ", c) ||
        !readDouble("Plane coefficient d: ", d))
    {
        return;
    }

    std::wstring buffer(512, L'\0');
    const auto required = GetPlaneEquationExport(a, b, c, d, buffer.data(), buffer.size());
    if (required < 0)
    {
        std::cout << "Failed to build plane equation.\n";
        return;
    }

    std::wcout << L"Equation: " << buffer.c_str() << L'\n';
}

void piInteractive()
{
    std::size_t iterations = 0U;
    if (!readSize("Gauss-Legendre iterations: ", iterations))
    {
        return;
    }

    std::cout << std::setprecision(16) << "pi ~= " << ComputePiExport(iterations) << '\n';
}

void calculusInteractive()
{
    MathEngineFunctionSpec spec {};
    if (!readFunctionSpec(spec))
    {
        return;
    }

    if (!describeFunction(spec))
    {
        return;
    }

    std::cout << "\nCalculus operations\n";
    std::cout << "1. Evaluate f(x)\n";
    std::cout << "2. First derivative f'(x)\n";
    std::cout << "3. Second derivative f''(x)\n";
    std::cout << "4. Definite integral\n";
    std::cout << "5. Limit\n";

    int operation = 0;
    if (!readInt("Choose operation: ", operation))
    {
        return;
    }

    switch (operation)
    {
    case 1:
    {
        double x = 0.0;
        if (!readDouble("Evaluate at x = ", x))
        {
            return;
        }

        std::cout << "f(" << x << ") = " << EvaluateFunctionExport(&spec, x) << '\n';
        break;
    }
    case 2:
    {
        double x = 0.0;
        if (!readDouble("Differentiate at x = ", x))
        {
            return;
        }

        std::cout << "f'(" << x << ") = " << DifferentiateFunctionExport(&spec, x) << '\n';
        break;
    }
    case 3:
    {
        double x = 0.0;
        if (!readDouble("Second derivative at x = ", x))
        {
            return;
        }

        std::cout << "f''(" << x << ") = " << SecondDerivativeFunctionExport(&spec, x) << '\n';
        break;
    }
    case 4:
    {
        double lower = 0.0;
        double upper = 0.0;
        if (!readDouble("Lower bound: ", lower) || !readDouble("Upper bound: ", upper))
        {
            return;
        }

        std::cout << "Integral = " << IntegrateFunctionExport(&spec, lower, upper) << '\n';
        break;
    }
    case 5:
    {
        double point = 0.0;
        int direction = 0;
        if (!readDouble("Limit point x -> ", point))
        {
            return;
        }

        std::cout << "Direction: -1 = left, 0 = two-sided, 1 = right\n";
        if (!readInt("Direction: ", direction))
        {
            return;
        }

        std::cout << "Limit = " << LimitFunctionExport(&spec, point, direction) << '\n';
        break;
    }
    default:
        std::cout << "Unknown calculus operation.\n";
        break;
    }
}

void maclaurinInteractive()
{
    std::cout << "\nMaclaurin approximations\n";
    std::cout << "1. exp(x)\n";
    std::cout << "2. sin(x)\n";
    std::cout << "3. cos(x)\n";

    int choice = 0;
    double x = 0.0;
    std::size_t terms = 0U;
    if (!readInt("Choose function: ", choice) || !readDouble("x = ", x) || !readSize("Number of terms: ", terms))
    {
        return;
    }

    double approximation = std::numeric_limits<double>::quiet_NaN();
    double reference = std::numeric_limits<double>::quiet_NaN();

    switch (choice)
    {
    case 1:
        approximation = MaclaurinExpExport(x, terms);
        reference = std::exp(x);
        break;
    case 2:
        approximation = MaclaurinSinExport(x, terms);
        reference = std::sin(x);
        break;
    case 3:
        approximation = MaclaurinCosExport(x, terms);
        reference = std::cos(x);
        break;
    default:
        std::cout << "Unknown series choice.\n";
        return;
    }

    std::cout << std::setprecision(16);
    std::cout << "Series approximation = " << approximation << '\n';
    std::cout << "Reference value      = " << reference << '\n';
    std::cout << "Absolute error       = " << std::abs(approximation - reference) << '\n';
}

void eigenvalueInteractive()
{
    std::size_t dimension = 0U;
    if (!readSize("Matrix dimension n for an n x n matrix: ", dimension) || dimension == 0U)
    {
        return;
    }

    std::vector<double> values(dimension * dimension, 0.0);
    for (std::size_t row = 0U; row < dimension; ++row)
    {
        for (std::size_t column = 0U; column < dimension; ++column)
        {
            if (!readDouble("A[" + std::to_string(row) + "][" + std::to_string(column) + "] = ",
                    values[row * dimension + column]))
            {
                return;
            }
        }
    }

    std::cout << "Dominant eigenvalue ~= " << DominantEigenvalueExport(values.data(), dimension) << '\n';
}

void goldenRatioInteractive()
{
    std::size_t iterations = 0U;
    if (!readSize("Continued fraction iterations: ", iterations))
    {
        return;
    }

    std::cout << std::setprecision(16) << "Golden ratio ~= " << ComputeGoldenRatioExport(iterations) << '\n';
}
} // namespace

int main()
{
    std::cout << std::setprecision(12);

    while (true)
    {
        printMenu();

        int choice = -1;
        if (!(std::cin >> choice))
        {
            clearInput();
            std::cout << "Invalid menu choice.\n";
            continue;
        }

        switch (choice)
        {
        case 1:
            solveQuadraticInteractive();
            break;
        case 2:
            solveCubicInteractive();
            break;
        case 3:
            statsInteractive();
            break;
        case 4:
            probabilityInteractive();
            break;
        case 5:
            circleEquationInteractive();
            break;
        case 6:
            planeEquationInteractive();
            break;
        case 7:
            piInteractive();
            break;
        case 8:
            calculusInteractive();
            break;
        case 9:
            maclaurinInteractive();
            break;
        case 10:
            eigenvalueInteractive();
            break;
        case 11:
            goldenRatioInteractive();
            break;
        case 0:
            return 0;
        default:
            std::cout << "Unknown option.\n";
            break;
        }
    }
}

