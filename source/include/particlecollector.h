#ifndef PARTICLECOLLECTOR_H
#define PARTICLECOLLECTOR_H

#include <string>

namespace Node {


class ParticleCollector
{
public:
    ParticleCollector(const std::string& directory, const std::string& name);

    auto collect(const std::string& /*name*/) -> void;

private:
    auto save() -> void;

    std::string m_directory {};
    std::string m_name {};
};

}

#endif // PARTICLECOLLECTOR_H
