#include <cmath>
#include <cstdlib>
#include <iostream>
#include <libconfig.h++>

#include "atmosphere_layers.h"

namespace Consts {
auto a() -> long double
{
    return (Consts::kappa - 1) / Consts::kappa * Consts::M * Consts::g / Consts::R;
}
} // namespace Consts

OptimisationTarget::~OptimisationTarget() = default;

auto Pressure::func(const long double& h) -> long double
{
    return Consts::p_0 * std::pow((1 - Consts::a() * h / Consts::T_0), Consts::M * Consts::g / (Consts::R * Consts::a()));
}

auto Pressure::integ(const long double& h) -> long double
{
    return -(func(h) * (Consts::kappa - 1) * (Consts::T_0 - Consts::a() * h)) / (Consts::a() * (2 * Consts::kappa - 1));
}

auto Pressure::integral(const long double& lower, const long double& upper) -> long double
{
    return integ(upper) - integ(lower);
}

auto Density::func(const long double& h) -> long double
{
    return Consts::rho_0 * std::pow((1 - Consts::a() * h / Consts::T_0), Consts::M * Consts::g / (Consts::R * Consts::a() * Consts::kappa));
}

auto Density::integ(const long double& h) -> long double
{
    return -(func(h) * (Consts::kappa - 1) * (Consts::T_0 - Consts::a() * h)) / (Consts::a() * Consts::kappa);
}

auto Density::integral(const long double& lower, const long double& upper) -> long double
{
    return integ(upper) - integ(lower);
}

auto Temperature::func(const long double& h) -> long double
{
    return Consts::T_0 * (1 - Consts::a() * h / Consts::T_0);
}

auto Temperature::integ(const long double& h) -> long double
{
    return Consts::T_0 * h - (Consts::a() * std::pow(h, 2)) / 2;
}

auto Temperature::integral(const long double& lower, const long double& upper) -> long double
{
    return integ(upper) - integ(lower);
}

Layer::Layer(const long double& lower, const long double& upper, OptimisationTarget* target, Layer* parent)
    : m_lower { lower }
    , m_upper { upper }
    , m_next { nullptr }
    , m_prev { parent }
    , m_target { target }
{
}

Layer::~Layer()
{
    delete m_target;
    delete m_next;
}

auto Layer::create(const long double& lower, const long double& upper, OptimisationTarget* target, const size_t& n) -> Layer*
{
    long double thickness = (upper - lower) / static_cast<long double>(n);
    long double h = lower + thickness;
    auto* first = new Layer(lower, h, target);
    Layer* last = first;
    for (size_t i = 1; i < n; i++) {
        last->m_next = new Layer(h, h + thickness, target, last);
        h += thickness;
        last = last->m_next;
    }
    return first;
}

auto Layer::total_lower() const -> long double
{
    if (m_prev != nullptr) {
        return m_prev->total_lower();
    }
    return m_lower;
}

auto Layer::total_upper() const -> long double
{
    if (m_next != nullptr) {
        return m_next->total_upper();
    }
    return m_upper;
}

void Layer::move(const long double& lower)
{
    if (m_next == nullptr) {
        m_lower = lower;
        return;
    }
    m_lower = lower;
    m_upper = lower + (total_upper() - m_lower) / total_n();
    m_next->move(m_upper);
}

void Layer::resize(const long double& thickness)
{
    if (m_next == nullptr) {
        return;
    }
    m_upper = m_lower + thickness;
    m_next->move(m_upper);
}

auto Layer::integral(const long double& lower, const long double& upper) const -> long double
{
    return m_target->integral(lower, upper);
}

auto Layer::total_integral() const -> long double
{
    return integral(total_lower(), total_upper());
}

auto Layer::average_integral() const -> long double
{
    return 1 / total_n(true) * total_integral();
}

auto Layer::total_n(const bool& all) const -> long double
{
    if (all) {
        if (m_prev == nullptr) {
            if (m_next != nullptr) {
                return m_next->total_n() + 1;
            }

            return 1;
        }
        return m_prev->total_n(true);
    }
    if (m_next != nullptr) {
        return m_next->total_n() + 1;
    }
    return 1;
}

auto Layer::fit_to_area(const long double& target) -> bool
{
    if (std::abs(integral(m_lower, m_upper) - target) < std::numeric_limits<long double>::epsilon()) {
        return true;
    }
    size_t i { 0 };
    do {
        resize(target / (integral(m_lower, m_upper) / thickness()));
        i++;
    } while ((std::abs(integral(m_lower, m_upper) - target) > std::numeric_limits<long double>::epsilon()) && (i < Consts::max_tries));
    return false;
}

auto Layer::thickness() const -> long double
{
    return m_upper - m_lower;
}

auto Layer::optimise(const size_t& j, const size_t& i) -> bool
{
    if (j >= (total_n())) // limit the number of tries for lower level layers
    {
        return false;
    }

    bool finished = fit_to_area(average_integral()); // try and adjust the layer thickness to the average area of all following layers
    if (m_next != nullptr) {
        m_next->optimise(0, i + 1);
    }
    if (std::abs(average_integral() - integral(m_lower, m_upper)) > std::numeric_limits<long double>::epsilon()) {
        return optimise(j + 1, i);
    }
    return finished;
}

void Layer::print()
{
    if (m_prev == nullptr) {
        std::cout << m_lower;
    }
    std::cout << "->" << m_upper;
    if (m_next != nullptr) {
        m_next->print();
    }
    if (m_prev == nullptr) {
        std::cout << '\n';
    }
}

void Layer::print_detailed(const size_t& n)
{
    Density rho;
    Pressure p;
    Temperature t;
    std::cout << "\nlayer " << std::to_string(n) << ":\n lower bound: " << m_lower << "m\n upper bound: " << m_upper << "m\n t = " << m_upper - m_lower << 'm';
    std::cout << "\n ρ = " << (rho.integral(m_lower, m_upper) / thickness()) << " kg/m^3\n p = " << (p.integral(m_lower, m_upper) / thickness()) << " Pa\n T = " << (t.integral(m_lower, m_upper) / thickness()) << " K\n";
    if (m_next != nullptr) {
        m_next->print_detailed(n + 1);
    }
}

void Layer::print_csv(const size_t& n)
{
    if (m_prev == nullptr) {
        std::cout << "n,lower,upper,density,pressure,termperature\n";
    }
    Density rho;
    Pressure p;
    Temperature t;
    std::cout << std::to_string(n) << ',' << m_lower << ',' << m_upper << ',' << rho.integral(m_lower, m_upper) / thickness() << ',' << p.integral(m_lower, m_upper) / thickness() << ',' << t.integral(m_lower, m_upper) / thickness() << '\n';
    if (m_next != nullptr) {
        m_next->print_csv(n + 1);
    }
}

void Layer::print_config(libconfig::Setting& root, const size_t& i)
{
    libconfig::Setting& layer = root.add(libconfig::Setting::TypeGroup);

    Density rho;
    Pressure p;
    Temperature t;
    layer.add("id", libconfig::Setting::TypeInt) = static_cast<int>(i);
    layer.add("lower", libconfig::Setting::TypeFloat) = static_cast<double>(m_lower);
    layer.add("upper", libconfig::Setting::TypeFloat) = static_cast<double>(m_upper);
    layer.add("density", libconfig::Setting::TypeFloat) = static_cast<double>(rho.integral(m_lower, m_upper) / thickness());
    layer.add("pressure", libconfig::Setting::TypeFloat) = static_cast<double>(p.integral(m_lower, m_upper) / thickness());
    layer.add("temperature", libconfig::Setting::TypeFloat) = static_cast<double>(t.integral(m_lower, m_upper) / thickness());

    if (m_next != nullptr) {
        m_next->print_config(root, i + 1);
    }
}

void Layer::print_config()
{
    libconfig::Config cfg;
    libconfig::Setting& root = cfg.getRoot();
    print_config(root.add("layers", libconfig::Setting::TypeList));
    cfg.write(stdout);
}

void print_help()
{
    std::cout << "possible parameters:\n\t-h\t\tprint this help\n\t-u <double>\tset upper limit unit: m\n\t\tdefault: 40000 m\n\t-T <double>\tSet temperature at sealevel unit: K\n\t\tdefault: 288.15 K\n\t-p <double>\tSet pressure at sealevel unit: Pa\n\t\tdefault: 101325 Pa\n\t-rho <double>\tSet density at sealevel unit: kg/m^3\n\t\tdefault: 1.225 kg/m^3\n\t-k <double>\tSet heat capacity ratio\n\t\tdefault: 1.235\n\t-n <int>\tset number of layers\n\t\tdefault: 10\n\t-csv\t\tgenerate csv formatted output\n\t-config\t\tgenerate libconfig formatted output\n";
}

auto main(int argc, char* argv[]) -> int
{
    bool csv = false;
    bool config = false;
    long double lower {};
    long double upper { Consts::Initial::height };
    size_t n { Consts::Initial::layers };
    if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            std::string arg(argv[i]);
            if (arg == "-csv") {
                csv = true;
            }
            if (arg == "-config") {
                config = true;
            } else if (arg == "-h") {
                print_help();
                return 0;
            } else if (arg == "-u") {
                if (++i >= argc) {
                    std::cout << "expected double after -u\n";
                    print_help();
                    return 1;
                }
                upper = std::stold(argv[i]);
            } else if (arg == "-T") {
                if (++i >= argc) {
                    std::cout << "expected double after -T\n";
                    print_help();
                    return 1;
                }
                Consts::T_0 = std::stold(argv[i]);
            } else if (arg == "-rho") {
                if (++i >= argc) {
                    std::cout << "expected double after -rho\n";
                    print_help();
                    return 1;
                }
                Consts::rho_0 = std::stold(argv[i]);
            } else if (arg == "-p") {
                if (++i >= argc) {
                    std::cout << "expected double after -p\n";
                    print_help();
                    return 1;
                }
                Consts::p_0 = std::stold(argv[i]);
            } else if (arg == "-k") {
                if (++i >= argc) {
                    std::cout << "expected double after -k\n";
                    print_help();
                    return 1;
                }
                Consts::kappa = std::stold(argv[i]);
            } else if (arg == "-n") {
                if (++i >= argc) {
                    std::cout << "expected integer after -n\n";
                    print_help();
                    return 1;
                }
                n = std::stoul(argv[i]);
            } else {
                std::cout << "invalid argument\n";
                print_help();
                return 1;
            }
        }
    }
    if (!csv && !config) {
        std::cout << "Calculating layers for parameters:\n\th_max = " << upper << " m\n\tρ_0 = " << Consts::rho_0 << " kg/m^3\n\tp_0 = " << Consts::p_0 << " Pa\n\tT_0 = " << Consts::T_0 << " K\n\tκ = " << Consts::kappa << "\n\tn = " << std::to_string(n) << '\n';
    }
    // lower bound, upper bound, layers
    Layer* layers = Layer::create(lower, upper, new Density(), n);
    layers->optimise();
    if (csv) {
        layers->print_csv();
    } else if (config) {
        layers->print_config();
    } else {
        layers->print_detailed();
    }
    delete layers;

    return 0;
}
