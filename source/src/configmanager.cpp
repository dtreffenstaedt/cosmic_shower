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

std::variant<std::vector<DetectorPlacement>, size_t> ConfigManager::get_detectors() const
{
    unsigned int number;
    if (!m_config.getRoot().lookupValue("detectors", number))
    {
        const libconfig::Setting& detectors_setting = (m_config.getRoot())["detectors"];
        std::vector<DetectorPlacement> detectors;
        size_t len = detectors_setting.getLength();
        for (size_t i = 0; i < len; i++)
        {
            const libconfig::Setting& detector = detectors_setting[i];
            DetectorPlacement settings;
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

std::vector<AtmosphereLayer> ConfigManager::get_atmosphere_layers() const
{
    const libconfig::Setting& layers_setting = (m_config.getRoot())["layers"];
    std::vector<AtmosphereLayer> layers;
    size_t len = layers_setting.getLength();
    for (size_t i = 0; i < len; i++)
    {
        const libconfig::Setting& layer = layers_setting[i];
        AtmosphereLayer settings;
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

std::vector<int> ConfigManager::get_particles() const
{
    const libconfig::Setting& particles_setting = (m_config.getRoot())["particles"];
    std::vector<int> particles;
    size_t len = particles_setting.getLength();
    for (size_t i = 0; i < len; i++)
    {
        particles.push_back(static_cast<int>(particles_setting[i]));
    }
    return particles;
}


PrimaryParticle ConfigManager::get_primary_particle() const
{
    const libconfig::Setting& primary_setting = (m_config.getRoot())["primary"];
    PrimaryParticle primary;
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
