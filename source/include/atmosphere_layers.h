#ifndef ATMOSPHERE_LAYERS_H
#define ATMOSPHERE_LAYERS_H

#include <cstddef>
#include <libconfig.h++>

namespace Consts {
static constexpr long double R = 8.31446261815324L;
static constexpr long double M = 28.96L / 1000.0L;
static constexpr long double g = 9.81L;
namespace Initial {
    static constexpr long double rho_0 = 1.225L;
    static constexpr long double p_0 = 101325L;
    static constexpr long double T_0 = 273.15L + 15.0L;
    static constexpr long double kappa = 1.235L;
    static constexpr long double height = 40000.0L;
    static constexpr size_t layers = 10;
}
static constexpr size_t max_tries = 10;

static long double rho_0 = Initial::rho_0;
static long double p_0 = Initial::p_0;
static long double T_0 = Initial::T_0;
static long double kappa = Initial::kappa;

[[nodiscard]] auto a() -> long double;
} // namespace Consts

class OptimisationTarget {
public:
    [[nodiscard]] virtual auto func(const long double& h) -> long double = 0;
    [[nodiscard]] virtual auto integral(const long double& lower, const long double& upper) -> long double = 0;

    virtual ~OptimisationTarget();
};

class Pressure : public OptimisationTarget {
public:
    [[nodiscard]] auto func(const long double& h) -> long double override;

    [[nodiscard]] auto integ(const long double& h) -> long double;

    [[nodiscard]] auto integral(const long double& lower, const long double& upper) -> long double override;
};

class Density : public OptimisationTarget {
public:
    [[nodiscard]] auto func(const long double& h) -> long double override;

    [[nodiscard]] auto integ(const long double& h) -> long double;

    [[nodiscard]] auto integral(const long double& lower, const long double& upper) -> long double override;
};

class Temperature : public OptimisationTarget {
public:
    [[nodiscard]] auto func(const long double& h) -> long double override;

    [[nodiscard]] static auto integ(const long double& h) -> long double;

    [[nodiscard]] auto integral(const long double& lower, const long double& upper) -> long double override;
};

class Layer {
public:
    Layer(const long double& lower, const long double& upper, OptimisationTarget* target, Layer* parent = nullptr);

    virtual ~Layer();

    [[nodiscard]] static auto create(const long double& lower, const long double& upper, OptimisationTarget* target, const std::size_t& n) -> Layer*;

    [[nodiscard]] auto total_lower() const -> long double;

    [[nodiscard]] auto total_upper() const -> long double;

    void move(const long double& lower);

    void resize(const long double& thickness);

    [[nodiscard]] auto integral(const long double& lower, const long double& upper) const -> long double;

    [[nodiscard]] auto total_integral() const -> long double;

    [[nodiscard]] auto average_integral() const -> long double;

    [[nodiscard]] auto total_n(const bool& all = false) const -> long double;

    [[nodiscard]] auto fit_to_area(const long double& target) -> bool;

    [[nodiscard]] auto thickness() const -> long double;

    auto optimise(const std::size_t& j = 0, const std::size_t& i = 1) -> bool;

    void print();

    void print_detailed(const std::size_t& n = 1);

    void print_csv(const std::size_t& n = 1);

    void print_config(libconfig::Setting& root, const std::size_t& i = 0);

    void print_config();

private:
    long double m_lower; // lower bound of layer
    long double m_upper; // upper bound of layer

    Layer* m_next;
    Layer* m_prev;

    OptimisationTarget* m_target;
};

void print_help();

#endif // ATMOSPHERE_LAYERS_H
