#include <iostream>
#include <cmath>
#include <cstdlib>


namespace Consts
{
    constexpr long double R = 8.31446261815324;
    constexpr long double M = 28.96/1000;
    constexpr long double g = 9.81;
    constexpr long double r_0 = 1.225;
    constexpr long double p_0 = 101325;
    constexpr long double T_0 = 273.15+15;
}

constexpr long double factor()
{
    return - (Consts::M * Consts::g / (Consts::R * Consts::T_0));
        
}
long double func(const long double& h)
{
    return std::exp(h*factor());
}

long double integral(const long double& lower, const long double& upper, const long double& offset)
{
    return std::abs(1/factor() * (func(upper) - func(lower)) - offset*(upper - lower));
}

long double integral(const long double& lower, const long double& upper)
{
    return (1/factor() * (func(upper) - func(lower)));
}


class Layer
{
public:
    Layer(const long double& lower, const long double& upper, Layer* parent = 0) :
        m_lower{lower},
        m_upper{upper},
        m_prev{parent}
    {
    }

    static Layer* create(const long double& lower, const long double& upper, const size_t& n)
    {
        long double thickness = (upper - lower)/static_cast<long double>(n);
        long double h = lower + thickness;
        Layer* first = new Layer(lower, h);
        Layer* last = first;
        for (size_t i = 1; i < n; i++)
        {
            last->m_next = new Layer(h, h+thickness, last);
            h += thickness;
            last = last->m_next;
        }
        return first;
    }

    virtual ~Layer() {}
    
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

    long double error() const
    {
        long double h = (m_upper - m_lower)/2 + m_lower;
        long double middle = integral(m_lower, m_upper)/thickness();
        return (integral(m_lower, h, middle) + integral(h, m_upper, middle))/thickness();
    }

    long double smallest_error() const
    {
        if (m_next)
        {
            long double c = m_next->smallest_error();
            long double e = error();
            if (e < c)
            {
                return e;
            }
            return c;
        }
        return error();
    }


    long double total_integral() const
    {
        return integral(total_lower(), total_upper());
    }

    long double average_integral() const
    {
        return 1/total_n(true) * total_integral();
    }

    long double biggest_error() const
    {
        if (m_next)
        {
            long double c = m_next->biggest_error();
            long double e = error();
            if (e < c)
            {
                return c;
            }
            return e;
        }
        return error();
    }

    long double total_error() const
    {
        if (m_next)
        {
            return m_next->total_error() + error();
        }
        return error();
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
        std::cout<<"\nlayer "<<std::to_string(n)<<":\n lower bound: "<<m_lower<<"m\n upper bound: "<<m_upper<<"m\n t = "<<m_upper - m_lower<<'m';
        std::cout<<"\n ρ = "<<(integral(m_lower, m_upper)/thickness())*Consts::r_0<<"kg/m^3\n p = "<<(integral(m_lower, m_upper)/thickness())*Consts::p_0<<"Pa\n f = "<<(integral(m_lower, m_upper)/thickness());
        std::cout<<"\n ε = "<<error()<<'\n';
        if (m_next)
        {
            m_next->print_detailed(n + 1);
        }
    }

    void print_csv(const size_t& n = 1)
    {    
        if (!m_prev)
        {
            std::cout<<"n,lower,upper,average,f\n";
        }
        std::cout<<std::to_string(n)<<','<<m_lower<<','<<m_upper<<','<<(m_lower + m_upper)/2<<','<<(integral(m_lower, m_upper)/thickness())<<'\n';
        if (m_next)
        {
            m_next->print_csv(n + 1);
        }
    }

private:
    long double m_lower;    // lower bound of layer
    long double m_upper;    // upper bound of layer

    Layer* m_next;
    Layer* m_prev;
};

void print_help()
{
    std::cout<<"possible parameters:\n\t-h\t\tprint this help\n\t-u <double>\tset upper limit\n\t-l <double>\tset lower limit\n\t-n <int>\tset number of layers\n\t--csv\t\tgenerate csv formatted output\n";
}

int main(int argc, char* argv[])
{
    bool csv = false;
    long double lower = 0;
    long double upper = 40000;
    size_t n = 12;
    if (argc >= 2)
    {
        for (int i = 1; i < argc; i++)
        {
            std::string arg(argv[i]);
            if (arg.compare("--csv") == 0)
            {
                csv = true;
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
            else if (arg.compare("-l") == 0)
            {
                if (++i >= argc)
                {
                    std::cout<<"expected double after -l\n";
                    print_help();
                    return 1;
                }
                lower = std::atof(argv[i]);
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
    if (!csv)
    {
        std::cout<<"Calculating layers for parameters:\n\th_min = "<<lower<<"m\n\th_max = "<<upper<<"\n\tn = "<<std::to_string(n)<<'\n';
    }
    // lower bound, upper bound, layers
    Layer* layers = Layer::create(lower, upper, n);            
    layers->optimise();                             
    if (csv)
    {
        layers->print_csv();
    }
    else
    {
        layers->print_detailed();

        std::cout<<"\nε_t = "<<layers->total_error()<<'\n';
        std::cout<<"ε_max = "<<layers->biggest_error()<<'\n';
        std::cout<<"ε_min = "<<layers->smallest_error()<<'\n';
    }

    return 0;
}


