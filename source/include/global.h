#ifndef cosmic_shower_GLOBAL_H
#define cosmic_shower_GLOBAL_H

#define SHOWER_BUILD_UI
/* #undef SHOWER_BENCHMARK */

#include <G4SystemOfUnits.hh>
#include <exception>
#include <globals.hh>

namespace Shower {
namespace Config {
    namespace Version {
        constexpr int major { 0 };
        constexpr int minor { 1 };
        constexpr int patch { 0 };
    } // namespace Version
    constexpr G4double max_energy { 1.0e20 * eV };
} // namespace Config
class DataDirectoryExists : public std::exception {
    [[nodiscard]] auto what() const noexcept -> const char* override
    {
        return "Data error: Output directory already exists. Exiting to preserve previous data.";
    }
};
} // namespace Shower

#endif // cosmic_shower_GLOBAL_H
