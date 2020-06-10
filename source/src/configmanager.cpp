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

std::variant<std::vector<DetectorSettings>, size_t> ConfigManager::get_detectors() const
{
    unsigned int number;
    if (!m_config.getRoot().lookupValue("detectors", number))
    {
        const libconfig::Setting& detectors_setting = (m_config.getRoot())["detectors"];
        std::vector<DetectorSettings> detectors;
        size_t len = detectors_setting.getLength();
        for (size_t i = 0; i < len; i++)
        {
            const libconfig::Setting& detector = detectors_setting[i];
            DetectorSettings settings;
            if (!(detector.lookupValue("x", settings.x) &&
                detector.lookupValue("y", settings.x) &&
                detector.lookupValue("z", settings.x) &&
                detector.lookupValue("pitch", settings.pitch) &&
                detector.lookupValue("yaw", settings.yaw) &&
                detector.lookupValue("roll", settings.roll) &&
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

std::vector<std::string> ConfigManager::get_particles() const
{
    const libconfig::Setting& particles_setting = (m_config.getRoot())["particles"];
    std::vector<std::string> particles;
    size_t len = particles_setting.getLength();
    for (size_t i = 0; i < len; i++)
    {
        particles.push_back(static_cast<std::string>(particles_setting[i]));
    }
    return particles;
}

}
