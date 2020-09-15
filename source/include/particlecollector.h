#ifndef PARTICLECOLLECTOR_H
#define PARTICLECOLLECTOR_H

#include <string>
#include <array>
#include <optional>
#include <variant>
#include <fstream>

namespace Node {

typedef std::variant<std::string, double, std::int32_t> variant_t;

struct DetailedHit {
    std::int32_t pdg{};
    std::string detector{};
    struct {
        double x;
        double y;
        double z;
    } position;
    struct {
        double x;
        double y;
        double z;
        double m;
    } energy;
    double global_time;
    double proper_time;
};

class ParticleCollector
{
public:
    ParticleCollector(const std::string& directory, const std::string& name);

    auto collect(const std::string& name) -> void;

private:
    auto save() -> void;

    auto collect(const std::string& name, std::ofstream& stream) -> void;

    auto collect_intensity(const std::string& dir) -> void;

    std::string m_directory {};
    std::string m_name {};

    std::ofstream m_detailed_hits {};
    std::ofstream m_hits {};
    std::ofstream m_ground_intensity {};
};

}

#endif // PARTICLECOLLECTOR_H
