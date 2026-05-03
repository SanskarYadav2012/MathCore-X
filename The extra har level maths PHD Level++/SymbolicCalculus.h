#pragma once

#include "MathCore.h"

namespace MathEngine::SymbolicCalculus
{
using VariableMap = std::unordered_map<std::string, long double>;

class Expression
{
public:
    using Evaluator = std::function<long double(const VariableMap&)>;

    Expression() = default;
    Expression(std::string description, Evaluator evaluator);

    [[nodiscard]] long double evaluate(const VariableMap& variables) const;
    [[nodiscard]] const std::string& description() const noexcept { return description_; }
    [[nodiscard]] bool empty() const noexcept { return !evaluator_; }

private:
    std::string description_ {};
    Evaluator evaluator_ {};
};

enum class LimitDirection
{
    Left,
    Right,
    TwoSided
};

class ISymbolicBackend
{
public:
    virtual ~ISymbolicBackend() = default;

    [[nodiscard]] virtual long double limit(
        const Expression& expression,
        const std::string& variable,
        long double point,
        LimitDirection direction) const = 0;

    [[nodiscard]] virtual long double derivative(
        const Expression& expression,
        const std::string& variable,
        long double at) const = 0;

    [[nodiscard]] virtual long double secondDerivative(
        const Expression& expression,
        const std::string& variable,
        long double at) const = 0;

    [[nodiscard]] virtual long double partialDerivative(
        const Expression& expression,
        const std::string& variable,
        const VariableMap& at) const = 0;

    [[nodiscard]] virtual long double integrateDefinite(
        const Expression& expression,
        const std::string& variable,
        long double lower,
        long double upper) const = 0;

    [[nodiscard]] virtual Expression integrateIndefinite(
        const Expression& expression,
        const std::string& variable,
        long double anchor) const = 0;
};

class NumericCalculusBackend final : public ISymbolicBackend
{
public:
    [[nodiscard]] long double limit(
        const Expression& expression,
        const std::string& variable,
        long double point,
        LimitDirection direction) const override;

    [[nodiscard]] long double derivative(
        const Expression& expression,
        const std::string& variable,
        long double at) const override;

    [[nodiscard]] long double secondDerivative(
        const Expression& expression,
        const std::string& variable,
        long double at) const override;

    [[nodiscard]] long double partialDerivative(
        const Expression& expression,
        const std::string& variable,
        const VariableMap& at) const override;

    [[nodiscard]] long double integrateDefinite(
        const Expression& expression,
        const std::string& variable,
        long double lower,
        long double upper) const override;

    [[nodiscard]] Expression integrateIndefinite(
        const Expression& expression,
        const std::string& variable,
        long double anchor) const override;
};

class CalculusEngine
{
public:
    CalculusEngine();
    explicit CalculusEngine(std::shared_ptr<ISymbolicBackend> backend);

    [[nodiscard]] long double limit(const Expression& expression, const std::string& variable, long double point,
        LimitDirection direction = LimitDirection::TwoSided) const;
    [[nodiscard]] long double derivative(const Expression& expression, const std::string& variable, long double at) const;
    [[nodiscard]] long double secondDerivative(const Expression& expression, const std::string& variable, long double at) const;
    [[nodiscard]] long double partialDerivative(const Expression& expression, const std::string& variable, const VariableMap& at) const;
    [[nodiscard]] long double integrateDefinite(const Expression& expression, const std::string& variable,
        long double lower, long double upper) const;
    [[nodiscard]] Expression integrateIndefinite(const Expression& expression, const std::string& variable,
        long double anchor = 0.0L) const;

private:
    std::shared_ptr<ISymbolicBackend> backend_ {};
};

#ifdef MATHENGINE_USE_SYMENGINE
class SymEngineBackend final : public ISymbolicBackend
{
public:
    [[nodiscard]] long double limit(
        const Expression& expression,
        const std::string& variable,
        long double point,
        LimitDirection direction) const override;

    [[nodiscard]] long double derivative(
        const Expression& expression,
        const std::string& variable,
        long double at) const override;

    [[nodiscard]] long double secondDerivative(
        const Expression& expression,
        const std::string& variable,
        long double at) const override;

    [[nodiscard]] long double partialDerivative(
        const Expression& expression,
        const std::string& variable,
        const VariableMap& at) const override;

    [[nodiscard]] long double integrateDefinite(
        const Expression& expression,
        const std::string& variable,
        long double lower,
        long double upper) const override;

    [[nodiscard]] Expression integrateIndefinite(
        const Expression& expression,
        const std::string& variable,
        long double anchor) const override;
};
#endif
} // namespace MathEngine::SymbolicCalculus

