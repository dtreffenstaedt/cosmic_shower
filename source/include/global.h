#ifndef cosmic_shower_GLOBAL_H
#define cosmic_shower_GLOBAL_H

/* #undef SHOWER_BUILD_UI */
/* #undef SHOWER_BENCHMARK */

#include <exception>

namespace Shower
{
namespace Config
{
namespace Version
{
    const int major = 0;
    const int minor = 1;
    const int patch = 0;
} // namespace Version
} // namespace Config
class DataDirectoryExists : public std::exception
{
    virtual const char* what() const throw()
    {
        return "Data error: Output directory already exists. Exiting to preserve previous data.";
    }
};
} // namespace Shower

#endif // cosmic_shower_GLOBAL_H
