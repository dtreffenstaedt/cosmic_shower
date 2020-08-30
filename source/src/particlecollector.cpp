#include "particlecollector.h"

#include <filesystem>

namespace Node {

ParticleCollector::ParticleCollector(const std::string& directory, const std::string& name)
    : m_directory { directory }
    , m_name { name }
{

}

auto ParticleCollector::collect(const std::string& /*name*/) -> void {
}

auto ParticleCollector::save() -> void {

}
}
