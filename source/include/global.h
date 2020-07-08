#ifndef cosmic_shower_GLOBAL_H
#define cosmic_shower_GLOBAL_H

#define START_NAMESPACE namespace SHOWER

/* #undef SHOWER_BUILD_UI */
#define SHOWER_BENCHMARK

#include <exception>

START_NAMESPACE
{
namespace Config
{
namespace Version
{
    const int major = 0;
    const int minor = 1;
    const int patch = 0;
}
}
class DataDirectoryExists : public std::exception
{
    virtual const char* what() const throw()
    {
        return "Data error: Output directory already exists. Exiting to preserve previous data.";
    }
};
}

#endif // cosmic_shower_GLOBAL_H
