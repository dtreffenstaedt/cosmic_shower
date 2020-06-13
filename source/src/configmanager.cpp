#include "configmanager.h"


START_NAMESPACE
{
ConfigManager* ConfigManager::c_singleton = 0;

ConfigManager* ConfigManager::singleton()
{
    return c_singleton;
}

ConfigManager::ConfigManager(const std::string& fileName)
{
    m_config.readFile(fileName.c_str());
    if (!m_config.getRoot().exists("detectors"))
    {
        throw NoDetectorsDefined();
    }

    c_singleton = this;
}

ConfigManager::~ConfigManager()
{}

std::variant<std::vector<Config::DetectorPlacement>, size_t> ConfigManager::get_detectors() const
{
    unsigned int number;
    if (!m_config.getRoot().lookupValue("detectors", number))
    {
        const libconfig::Setting& detectors_setting = (m_config.getRoot())["detectors"];
        std::vector<Config::DetectorPlacement> detectors;
        size_t len = detectors_setting.getLength();
        for (size_t i = 0; i < len; i++)
        {
            const libconfig::Setting& detector = detectors_setting[i];
            Config::DetectorPlacement settings;
            if (!(detector.lookupValue("x", settings.x) &&
                detector.lookupValue("y", settings.y) &&
                detector.lookupValue("z", settings.z) &&
                detector.lookupValue("name", settings.name))
                )
            {
                throw FaultyDetectorDefinition();
            }
            detectors.push_back(settings);
        }
        return detectors;
    }
    return static_cast<size_t>(number);
}

std::vector<Config::AtmosphereLayer> ConfigManager::get_atmosphere_layers() const
{
    const libconfig::Setting& layers_setting = (m_config.getRoot())["layers"];
    std::vector<Config::AtmosphereLayer> layers;
    size_t len = layers_setting.getLength();
    for (size_t i = 0; i < len; i++)
    {
        const libconfig::Setting& layer = layers_setting[i];
        Config::AtmosphereLayer settings;
        if (!(layer.lookupValue("lower", settings.lower) &&
              layer.lookupValue("upper", settings.upper) &&
              layer.lookupValue("density", settings.density) &&
              layer.lookupValue("pressure", settings.pressure) &&
              layer.lookupValue("temperature", settings.temperature))
            )
        {
            throw FaultyAtmosphereLayerDefinition();
        }
        layers.push_back(settings);
    }
    return layers;
}

std::vector<Config::SecondaryParticle> ConfigManager::get_particles() const
{
    const libconfig::Setting& particles_setting = (m_config.getRoot())["particles"];
    std::vector<Config::SecondaryParticle> particles;
    size_t len = particles_setting.getLength();
    for (size_t i = 0; i < len; i++)
    {
        const libconfig::Setting& particle = particles_setting[i];
        Config::SecondaryParticle settings;
        if (!(particle.lookupValue("pdg", settings.pdg) &&
              particle.lookupValue("cut", settings.cut))
            )
        {
            throw FaultySecondaryDefinition();
        }
        particles.push_back(settings);
    }
    return particles;
}


Config::PrimaryParticle ConfigManager::get_primary_particle() const
{
    const libconfig::Setting& primary_setting = (m_config.getRoot())["primary"];
    Config::PrimaryParticle primary;
    const libconfig::Setting& origin = primary_setting["origin"];
    const libconfig::Setting& momentum = primary_setting["momentum"];

    if (!(origin.lookupValue("x", primary.origin.x) &&
        origin.lookupValue("y", primary.origin.y) &&
        origin.lookupValue("z", primary.origin.z) &&
        momentum.lookupValue("x", primary.momentum.y) &&
        momentum.lookupValue("y", primary.momentum.y) &&
        momentum.lookupValue("z", primary.momentum.z) &&
        momentum.lookupValue("magnitude", primary.momentum.m) &&
        primary_setting.lookupValue("particle", primary.particle)))
    {
        throw FaultyPrimaryDefinition();
    }
    return primary;
}

}
