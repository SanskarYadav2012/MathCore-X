#include "pch.h"

#include "SymbolicCalculus.h"

namespace MathEngine::SymbolicCalculus
{
namespace
{
long double chooseStep(const long double value) noexcept
{
    return std::cbrt(std::numeric_limits<long double>::epsilon()) * (std::max)(1.0L, std::abs(value));
}

long double evaluateWithVariable(const Expression& expression, VariableMap variables,
    const std::string& variable, const long double value)
{
    variables[variable] = value;
    return expression.evaluate(variables);
}

long double adaptiveSimpsonRecursive(
    const std::function<long double(long double)>& function,
    const long double left,
    const long double right,
    const long double epsilon,
    const long double whole,
    const int depth)
{
    const auto midpoint = (left + right) / 2.0L;
    const auto leftMid = (left + midpoint) / 2.0L;
    const auto rightMid = (midpoint + right) / 2.0L;

    const auto leftArea = (midpoint - left) * (function(left) + 4.0L * function(leftMid) + function(midpoint)) / 6.0L;
    const auto rightArea = (right - midpoint) * (function(midpoint) + 4.0L * function(rightMid) + function(right)) / 6.0L;

    const auto refinement = leftArea + rightArea - whole;
    if (depth <= 0 || std::abs(refinement) < 15.0L * epsilon)
    {
        // Richardson extrapolation improves the final composite Simpson estimate.
        return leftArea + rightArea + refinement / 15.0L;
    }

    return adaptiveSimpsonRecursive(function, left, midpoint, epsilon / 2.0L, leftArea, depth - 1)
        + adaptiveSimpsonRecursive(function, midpoint, right, epsilon / 2.0L, rightArea, depth - 1);
}
} // namespace

Expression::Expression(std::string description, Evaluator evaluator)
    : description_(std::move(description)), evaluator_(std::move(evaluator))
{
    if (!evaluator_)
    {
        throw std::invalid_argument("Expression evaluator must be callable.");
    }
}

long double Expression::evaluate(const VariableMap& variables) const
{
    if (!evaluator_)
    {
        throw std::logic_error("Cannot evaluate an empty expression.");
    }

    return evaluator_(variables);
}

long double NumericCalculusBackend::limit(
    const Expression& expression,
    const std::string& variable,
    const long double point,
    const LimitDirection direction) const
{
    auto estimateSide = [&](const long double sign)
    {
        long double previous = 0.0L;
        bool initialized = false;

        for (int iteration = 0; iteration < 12; ++iteration)
        {
            const auto step = std::pow(0.5L, iteration + 1) * (std::max)(1.0L, std::abs(point));
            VariableMap variables;
            const auto value = evaluateWithVariable(expression, std::move(variables), variable, point + sign * step);

            if (initialized && std::abs(value - previous) < 1.0e-8L)
            {
                return value;
            }

            previous = value;
            initialized = true;
        }

        return previous;
    };

    if (direction == LimitDirection::Left)
    {
        return estimateSide(-1.0L);
    }

    if (direction == LimitDirection::Right)
    {
        return estimateSide(1.0L);
    }

    const auto left = estimateSide(-1.0L);
    const auto right = estimateSide(1.0L);
    if (std::abs(left - right) > 1.0e-6L)
    {
        return std::numeric_limits<long double>::quiet_NaN();
    }

    return (left + right) / 2.0L;
}

long double NumericCalculusBackend::derivative(
    const Expression& expression,
    const std::string& variable,
    const long double at) const
{
    const auto h = chooseStep(at);
    VariableMap variables;

    // The five-point stencil cancels the leading O(h^2) truncation error, leaving O(h^4).
    const auto fPlus2 = evaluateWithVariable(expression, variables, variable, at + 2.0L * h);
    const auto fPlus1 = evaluateWithVariable(expression, variables, variable, at + h);
    const auto fMinus1 = evaluateWithVariable(expression, variables, variable, at - h);
    const auto fMinus2 = evaluateWithVariable(expression, variables, variable, at - 2.0L * h);

    return (-fPlus2 + 8.0L * fPlus1 - 8.0L * fMinus1 + fMinus2) / (12.0L * h);
}

long double NumericCalculusBackend::secondDerivative(
    const Expression& expression,
    const std::string& variable,
    const long double at) const
{
    const auto h = chooseStep(at);
    VariableMap variables;

    const auto f = evaluateWithVariable(expression, variables, variable, at);
    const auto fPlus1 = evaluateWithVariable(expression, variables, variable, at + h);
    const auto fMinus1 = evaluateWithVariable(expression, variables, variable, at - h);
    const auto fPlus2 = evaluateWithVariable(expression, variables, variable, at + 2.0L * h);
    const auto fMinus2 = evaluateWithVariable(expression, variables, variable, at - 2.0L * h);

    return (-fPlus2 + 16.0L * fPlus1 - 30.0L * f + 16.0L * fMinus1 - fMinus2) / (12.0L * h * h);
}

long double NumericCalculusBackend::partialDerivative(
    const Expression& expression,
    const std::string& variable,
    const VariableMap& at) const
{
    const auto iterator = at.find(variable);
    if (iterator == at.end())
    {
        throw std::invalid_argument("Partial derivative requires the target variable in the evaluation point.");
    }

    const auto h = chooseStep(iterator->second);
    auto variables = at;

    const auto sample = [&](const long double offset)
    {
        variables[variable] = iterator->second + offset;
        return expression.evaluate(variables);
    };

    return (-sample(2.0L * h) + 8.0L * sample(h) - 8.0L * sample(-h) + sample(-2.0L * h)) / (12.0L * h);
}

long double NumericCalculusBackend::integrateDefinite(
    const Expression& expression,
    const std::string& variable,
    const long double lower,
    const long double upper) const
{
    if (lower == upper)
    {
        return 0.0L;
    }

    const auto function = [&](const long double value)
    {
        VariableMap variables;
        return evaluateWithVariable(expression, std::move(variables), variable, value);
    };

    const auto simpsonWhole = (upper - lower) * (function(lower) + 4.0L * function((lower + upper) / 2.0L) + function(upper)) / 6.0L;
    return adaptiveSimpsonRecursive(function, lower, upper, 1.0e-9L, simpsonWhole, 20);
}

Expression NumericCalculusBackend::integrateIndefinite(
    const Expression& expression,
    const std::string& variable,
    const long double anchor) const
{
    auto backend = std::make_shared<NumericCalculusBackend>(*this);

    return Expression(
        "Integral(" + expression.description() + ") d" + variable,
        [backend, expression, variable, anchor](VariableMap variables)
        {
            const auto iterator = variables.find(variable);
            if (iterator == variables.end())
            {
                throw std::invalid_argument("Indefinite integral evaluation requires the free variable.");
            }

            const auto upper = iterator->second;
            if (upper == anchor)
            {
                return 0.0L;
            }

            const auto integrand = Expression(
                expression.description(),
                [expression, variable, &variables](const VariableMap& local)
                {
                    auto merged = variables;
                    merged[variable] = local.at(variable);
                    return expression.evaluate(merged);
                });

            return backend->integrateDefinite(integrand, variable, anchor, upper);
        });
}

CalculusEngine::CalculusEngine()
    : backend_(std::make_shared<NumericCalculusBackend>())
{
}

CalculusEngine::CalculusEngine(std::shared_ptr<ISymbolicBackend> backend)
    : backend_(std::move(backend))
{
    if (!backend_)
    {
        throw std::invalid_argument("CalculusEngine requires a backend.");
    }
}

long double CalculusEngine::limit(const Expression& expression, const std::string& variable,
    const long double point, const LimitDirection direction) const
{
    return backend_->limit(expression, variable, point, direction);
}

long double CalculusEngine::derivative(const Expression& expression, const std::string& variable, const long double at) const
{
    return backend_->derivative(expression, variable, at);
}

long double CalculusEngine::secondDerivative(const Expression& expression, const std::string& variable, const long double at) const
{
    return backend_->secondDerivative(expression, variable, at);
}

long double CalculusEngine::partialDerivative(const Expression& expression, const std::string& variable, const VariableMap& at) const
{
    return backend_->partialDerivative(expression, variable, at);
}

long double CalculusEngine::integrateDefinite(const Expression& expression, const std::string& variable,
    const long double lower, const long double upper) const
{
    return backend_->integrateDefinite(expression, variable, lower, upper);
}

Expression CalculusEngine::integrateIndefinite(const Expression& expression, const std::string& variable, const long double anchor) const
{
    return backend_->integrateIndefinite(expression, variable, anchor);
}

#ifdef MATHENGINE_USE_SYMENGINE
long double SymEngineBackend::limit(
    const Expression& expression,
    const std::string& variable,
    const long double point,
    const LimitDirection direction) const
{
    return NumericCalculusBackend {}.limit(expression, variable, point, direction);
}

long double SymEngineBackend::derivative(
    const Expression& expression,
    const std::string& variable,
    const long double at) const
{
    return NumericCalculusBackend {}.derivative(expression, variable, at);
}

long double SymEngineBackend::secondDerivative(
    const Expression& expression,
    const std::string& variable,
    const long double at) const
{
    return NumericCalculusBackend {}.secondDerivative(expression, variable, at);
}

long double SymEngineBackend::partialDerivative(
    const Expression& expression,
    const std::string& variable,
    const VariableMap& at) const
{
    return NumericCalculusBackend {}.partialDerivative(expression, variable, at);
}

long double SymEngineBackend::integrateDefinite(
    const Expression& expression,
    const std::string& variable,
    const long double lower,
    const long double upper) const
{
    return NumericCalculusBackend {}.integrateDefinite(expression, variable, lower, upper);
}

Expression SymEngineBackend::integrateIndefinite(
    const Expression& expression,
    const std::string& variable,
    const long double anchor) const
{
    return NumericCalculusBackend {}.integrateIndefinite(expression, variable, anchor);
}
#endif
} // namespace MathEngine::SymbolicCalculus
