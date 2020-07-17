#include <iostream>
#include <cmath>
#include <cstdlib>
#include <libconfig.h++>


namespace Consts
{
    constexpr long double R = 8.31446261815324;
    constexpr long double M = 28.96/1000;
    constexpr long double g = 9.81;
    constexpr long double i_rho_0 = 1.225;
    constexpr long double i_p_0 = 101325;
    constexpr long double i_T_0 = 273.15+15;
    constexpr long double i_kappa = 1.235;

    long double rho_0 = i_rho_0;
    long double p_0 = i_p_0;
    long double T_0 = i_T_0;
    long double kappa = i_kappa;

    auto a() -> long double
    {
        return (Consts::kappa - 1)/Consts::kappa * Consts::M*Consts::g/Consts::R;
    }
} // namespace Consts

class OptimisationTarget
{
public:
    virtual long double func(const long double& h) = 0;
    virtual long double integral(const long double& lower, const long double& upper) = 0;

    virtual ~OptimisationTarget() {}
};

class Pressure : public OptimisationTarget
{
public:
    virtual long double func(const long double& h)
    {
        return Consts::p_0 * std::pow((1 - Consts::a() * h/Consts::T_0), Consts::M*Consts::g/(Consts::R*Consts::a()));
    }

    long double integ(const long double& h)
    {
        return -(func(h) * (Consts::kappa - 1) * (Consts::T_0 - Consts::a() * h))/(Consts::a() * (2 * Consts::kappa - 1));
    }

    virtual long double integral(const long double& lower, const long double& upper)
    {
        return integ(upper) - integ(lower);
    }
};

class Density : public OptimisationTarget
{
public:
    virtual long double func(const long double& h)
    {
        return Consts::rho_0 * std::pow((1 - Consts::a() * h/Consts::T_0), Consts::M*Consts::g/(Consts::R*Consts::a()*Consts::kappa));
    }


    long double integ(const long double& h)
    {
        return -(func(h)*(Consts::kappa - 1)*(Consts::T_0 - Consts::a()*h))/(Consts::a() * Consts::kappa);
    }

    virtual long double integral(const long double& lower, const long double& upper)
    {
        return integ(upper) - integ(lower);
    }
};

class Temperature : public OptimisationTarget
{
public:
    virtual long double func(const long double& h)
    {
        return Consts::T_0 * (1 - Consts::a() * h/Consts::T_0);
    }

    long double integ(const long double& h)
    {
        return Consts::T_0 * h - (Consts::a()*std::pow(h, 2))/2;
    }

    virtual long double integral(const long double& lower, const long double& upper)
    {
        return integ(upper) - integ(lower);
    }
};

class Layer
{
public:
    Layer(const long double& lower, const long double& upper, OptimisationTarget* target, Layer* parent = 0) :
        m_lower{lower},
        m_upper{upper},
        m_prev{parent},
        m_target{target}
    {
    }

    virtual ~Layer()
    {
        if (!m_prev)
        {
            delete m_target;
        }
        if (m_next)
        {
            delete m_next;
        }
    }

    static Layer* create(const long double& lower, const long double& upper, OptimisationTarget* target, const size_t& n)
    {
        long double thickness = (upper - lower)/static_cast<long double>(n);
        long double h = lower + thickness;
        Layer* first = new Layer(lower, h, target);
        Layer* last = first;
        for (size_t i = 1; i < n; i++)
        {
            last->m_next = new Layer(h, h+thickness, target, last);
            h += thickness;
            last = last->m_next;
        }
        return first;
    }

    long double total_lower() const
    {
        if (m_prev)
        {
            return m_prev->total_lower();
        }
        return m_lower;
    }

    long double total_upper() const
    {
        if (m_next)
        {
            return m_next->total_upper();
        }
        return m_upper;
    }

    void move(const long double& lower)
    {
        if (!m_next)
        {
            m_lower = lower;
            return;
        }
        m_lower = lower;
        m_upper = lower + (total_upper() - m_lower)/total_n();
        m_next->move(m_upper);
    }

    void resize(const long double& thickness)
    {
        if (!m_next)
        {
            return;
        }
        m_upper = m_lower + thickness;
        m_next->move(m_upper);
    }

    long double integral(const long double& lower, const long double& upper) const
    {
        return m_target->integral(lower, upper);
    }

    long double total_integral() const
    {
        return integral(total_lower(), total_upper());
    }

    long double average_integral() const
    {
        return 1/total_n(true) * total_integral();
    }

    long double total_n(const bool& all = false) const
    {
        if (all)
        {
            if (!m_prev)
            {
                return m_next->total_n() + 1;
            }
            return m_prev->total_n(true);
        }
        if (m_next)
        {
            return m_next->total_n() + 1;
        }
        return 1;
    }

    bool fit_to_area(const long double& target)
    {
        if (std::abs(integral(m_lower, m_upper) - target) < std::numeric_limits<long double>::epsilon())
        {
            return true;
        }
        size_t i = 0;
        do
        {
            resize(target / (integral(m_lower, m_upper)/thickness()));
            i++;
        } while ((std::abs(integral(m_lower, m_upper)-target) > std::numeric_limits<long double>::epsilon()) && (i < 10));
        return false;
    }

    long double thickness() const
    {
        return m_upper - m_lower;
    }

    bool optimise(const size_t& j = 0, const size_t& i = 1)
    {
        if (j >= (total_n())) // limit the number of tries for lower level layers
        {
            return false;
        }

        bool finished = fit_to_area(average_integral()); // try and adjust the layer thickness to the average area of all following layers
        if (m_next)
        {
            m_next->optimise(0, i +1);
        }
        if (std::abs(average_integral() - integral(m_lower, m_upper)) > std::numeric_limits<long double>::epsilon())
        {
            return optimise(j + 1, i);
        }
        return finished;
    }

    void print()
    {
        if (!m_prev)
        {
            std::cout<<m_lower;
        }
        std::cout<<"->"<<m_upper;
        if (m_next)
        {
            m_next->print();
        }
        if (!m_prev)
        {
            std::cout<<'\n';
        }
    }

    void print_detailed(const size_t& n = 1)
    {
        Density rho;
        Pressure p;
        Temperature t;
        std::cout<<"\nlayer "<<std::to_string(n)<<":\n lower bound: "<<m_lower<<"m\n upper bound: "<<m_upper<<"m\n t = "<<m_upper - m_lower<<'m';
        std::cout<<"\n ρ = "<<(rho.integral(m_lower, m_upper)/thickness())<<" kg/m^3\n p = "<<(p.integral(m_lower, m_upper)/thickness())<<" Pa\n T = "<<(t.integral(m_lower, m_upper)/thickness())<<" K\n";
        if (m_next)
        {
            m_next->print_detailed(n + 1);
        }
    }

    void print_csv(const size_t& n = 1)
    {
        if (!m_prev)
        {
            std::cout<<"n,lower,upper,density,pressure,termperature\n";
        }
        Density rho;
        Pressure p;
        Temperature t;
        std::cout<<std::to_string(n)<<','<<m_lower<<','<<m_upper<<','<<rho.integral(m_lower, m_upper)/thickness()<<','<<p.integral(m_lower, m_upper)/thickness()<<','<<t.integral(m_lower, m_upper)/thickness()<<'\n';
        if (m_next)
        {
            m_next->print_csv(n + 1);
        }
    }

    void print_config(libconfig::Setting& root, const size_t& i = 0)
    {
        libconfig::Setting& layer = root.add(libconfig::Setting::TypeGroup);

        Density rho;
        Pressure p;
        Temperature t;
        layer.add("id", libconfig::Setting::TypeInt) = static_cast<int>(i);
        layer.add("lower", libconfig::Setting::TypeFloat) = static_cast<double>(m_lower);
        layer.add("upper", libconfig::Setting::TypeFloat) = static_cast<double>(m_upper);
        layer.add("density", libconfig::Setting::TypeFloat) = static_cast<double>(rho.integral(m_lower, m_upper)/thickness());
        layer.add("pressure", libconfig::Setting::TypeFloat) = static_cast<double>(p.integral(m_lower, m_upper)/thickness());
        layer.add("temperature", libconfig::Setting::TypeFloat) = static_cast<double>(t.integral(m_lower, m_upper)/thickness());

        if (m_next)
        {
            m_next->print_config(root, i + 1);
        }
    }

    void print_config()
    {
        libconfig::Config cfg;
        libconfig::Setting& root = cfg.getRoot();
        print_config(root.add("layers", libconfig::Setting::TypeList));
        cfg.write(stdout);
    }

private:
    long double m_lower;    // lower bound of layer
    long double m_upper;    // upper bound of layer

    Layer* m_next;
    Layer* m_prev;

    OptimisationTarget* m_target;
};

void print_help()
{
    std::cout<<"possible parameters:\n\t-h\t\tprint this help\n\t-u <double>\tset upper limit unit: m\n\t\tdefault: 40000 m\n\t-T <double>\tSet temperature at sealevel unit: K\n\t\tdefault: 288.15 K\n\t-p <double>\tSet pressure at sealevel unit: Pa\n\t\tdefault: 101325 Pa\n\t-rho <double>\tSet density at sealevel unit: kg/m^3\n\t\tdefault: 1.225 kg/m^3\n\t-k <double>\tSet heat capacity ratio\n\t\tdefault: 1.235\n\t-n <int>\tset number of layers\n\t\tdefault: 10\n\t-csv\t\tgenerate csv formatted output\n\t-config\t\tgenerate libconfig formatted output\n";
}

int main(int argc, char* argv[])
{
    bool csv = false;
    bool config = false;
    long double lower = 0;
    long double upper = 40000;
    size_t n = 10;
    if (argc >= 2)
    {
        for (int i = 1; i < argc; i++)
        {
            std::string arg(argv[i]);
            if (arg.compare("-csv") == 0)
            {
                csv = true;
            }
            if (arg.compare("-config") == 0)
            {
                config = true;
            }
            else if (arg.compare("-h") == 0)
            {
                print_help();
                return 0;
            }
            else if (arg.compare("-u") == 0)
            {
                if (++i >= argc)
                {
                    std::cout<<"expected double after -u\n";
                    print_help();
                    return 1;
                }
                upper = std::atof(argv[i]);
            }
            else if (arg.compare("-T") == 0)
            {
                if (++i >= argc)
                {
                    std::cout<<"expected double after -T\n";
                    print_help();
                    return 1;
                }
                Consts::T_0 = std::atof(argv[i]);
            }
            else if (arg.compare("-rho") == 0)
            {
                if (++i >= argc)
                {
                    std::cout<<"expected double after -rho\n";
                    print_help();
                    return 1;
                }
                Consts::rho_0 = std::atof(argv[i]);
            }
            else if (arg.compare("-p") == 0)
            {
                if (++i >= argc)
                {
                    std::cout<<"expected double after -p\n";
                    print_help();
                    return 1;
                }
                Consts::p_0 = std::atof(argv[i]);
            }
            else if (arg.compare("-k") == 0)
            {
                if (++i >= argc)
                {
                    std::cout<<"expected double after -k\n";
                    print_help();
                    return 1;
                }
                Consts::kappa = std::atof(argv[i]);
            }
            else if (arg.compare("-n") == 0)
            {
                if (++i >= argc)
                {
                    std::cout<<"expected integer after -n\n";
                    print_help();
                    return 1;
                }
                n = std::atoi(argv[i]);
            }
            else
            {
                std::cout<<"invalid argument\n";
                print_help();
                return 1;
            }
        }
    }
    if (!csv && !config)
    {
        std::cout<<"Calculating layers for parameters:\n\th_max = "<<upper<<" m\n\tρ_0 = "<<Consts::rho_0<<" kg/m^3\n\tp_0 = "<<Consts::p_0<<" Pa\n\tT_0 = "<<Consts::T_0<<" K\n\tκ = "<<Consts::kappa<<"\n\tn = "<<std::to_string(n)<<'\n';
    }
    // lower bound, upper bound, layers
    Layer* layers = Layer::create(lower, upper, new Density(), n);
    layers->optimise();
    if (csv)
    {
        layers->print_csv();
    }
    else if (config)
    {
        layers->print_config();
    }
    else
    {
        layers->print_detailed();
    }
    delete layers;

    return 0;
}


