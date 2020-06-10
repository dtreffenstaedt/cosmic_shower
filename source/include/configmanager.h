#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "global.h"
#include <libconfig.h++>
#include <string>
#include <vector>

START_NAMESPACE
{
struct DetectorSettings
{
    double x;
    double y;
    double z;
    double pitch;
    double yaw;
    double roll;

    std::string name;
};


class NoDetectorsDefined : public std::exception
{
    virtual const char* what() const throw()
    {
        return "Config error: No detectors defined.";
    }
};

class FaultyDetectorDefinition : public std::exception
{
    virtual const char* what() const throw()
    {
        return "Config error: Faulty detector definition.";
    }
};

class ConfigManager
{
public:
    ConfigManager(const std::string& fileName);
    virtual ~ConfigManager();

    static ConfigManager* singleton();

    std::vector<DetectorSettings> get_detectors() const;
private:

    libconfig::Config m_config;

    static ConfigManager* c_singleton;
};
}

#endif // CONFIGMANAGER_H
