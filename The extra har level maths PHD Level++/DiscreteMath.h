#pragma once

#include "MathCore.h"

namespace MathEngine::DiscreteMath
{
template <typename T, typename Hash = std::hash<T>, typename KeyEqual = std::equal_to<T>>
class Set
{
public:
    Set() = default;
    Set(std::initializer_list<T> values)
        : values_(values)
    {
    }

    explicit Set(const std::vector<T>& values)
        : values_(values.begin(), values.end())
    {
    }

    [[nodiscard]] bool contains(const T& value) const
    {
        return values_.find(value) != values_.end();
    }

    void insert(const T& value)
    {
        values_.insert(value);
    }

    void erase(const T& value)
    {
        values_.erase(value);
    }

    [[nodiscard]] std::size_t size() const noexcept
    {
        return values_.size();
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return values_.empty();
    }

    [[nodiscard]] std::vector<T> values() const
    {
        return { values_.begin(), values_.end() };
    }

    [[nodiscard]] Set setUnion(const Set& other) const
    {
        Set result = *this;
        result.values_.insert(other.values_.begin(), other.values_.end());
        return result;
    }

    [[nodiscard]] Set intersection(const Set& other) const
    {
        Set result;
        const auto& smaller = values_.size() < other.values_.size() ? values_ : other.values_;
        const auto& larger = values_.size() < other.values_.size() ? other.values_ : values_;

        for (const auto& value : smaller)
        {
            if (larger.find(value) != larger.end())
            {
                result.values_.insert(value);
            }
        }

        return result;
    }

    [[nodiscard]] Set difference(const Set& other) const
    {
        Set result;
        for (const auto& value : values_)
        {
            if (other.values_.find(value) == other.values_.end())
            {
                result.values_.insert(value);
            }
        }

        return result;
    }

    [[nodiscard]] Set symmetricDifference(const Set& other) const
    {
        return difference(other).setUnion(other.difference(*this));
    }

private:
    std::unordered_set<T, Hash, KeyEqual> values_ {};
};

template <typename DomainT, typename CodomainT, typename HashD = std::hash<DomainT>,
    typename HashC = std::hash<CodomainT>, typename EqD = std::equal_to<DomainT>, typename EqC = std::equal_to<CodomainT>>
struct MappingReport
{
    std::unordered_map<DomainT, CodomainT, HashD, EqD> mapping {};
    bool isTotal {};
    bool isInjective {};
    bool isSurjective {};

    [[nodiscard]] bool isBijective() const noexcept
    {
        return isInjective && isSurjective;
    }
};

template <typename DomainT, typename CodomainT, typename Func, typename HashD = std::hash<DomainT>,
    typename HashC = std::hash<CodomainT>, typename EqD = std::equal_to<DomainT>, typename EqC = std::equal_to<CodomainT>>
[[nodiscard]] MappingReport<DomainT, CodomainT, HashD, HashC, EqD, EqC> analyzeMapping(
    const std::vector<DomainT>& domain,
    const std::vector<CodomainT>& codomain,
    Func&& mappingRule)
{
    if (domain.empty() || codomain.empty())
    {
        throw std::invalid_argument("Finite domain and codomain must both be non-empty.");
    }

    MappingReport<DomainT, CodomainT, HashD, HashC, EqD, EqC> report {};
    report.mapping.reserve(domain.size());

    std::unordered_set<CodomainT, HashC, EqC> codomainValues(codomain.begin(), codomain.end());
    std::unordered_set<CodomainT, HashC, EqC> image;
    image.reserve(domain.size());

    report.isTotal = true;
    report.isInjective = true;

    for (const auto& input : domain)
    {
        const auto output = std::invoke(mappingRule, input);
        if (codomainValues.find(output) == codomainValues.end())
        {
            report.isTotal = false;
            report.isInjective = false;
            report.isSurjective = false;
            return report;
        }

        report.mapping.emplace(input, output);
        if (!image.insert(output).second)
        {
            report.isInjective = false;
        }
    }

    report.isSurjective = image.size() == codomainValues.size();
    return report;
}
} // namespace MathEngine::DiscreteMath

