#ifndef cosmic_shower_GLOBAL_H
#define cosmic_shower_GLOBAL_H

/* #undef SHOWER_BUILD_UI */
#define SHOWER_BENCHMARK

#include <G4SystemOfUnits.hh>
#include <exception>
#include <globals.hh>
#include <memory>

namespace Shower {
namespace Config {
    namespace Version {
        constexpr int major { 0 };
        constexpr int minor { 1 };
        constexpr int patch { 0 };
    } // namespace Version
    constexpr G4double max_energy { 1.0e20 * eV };
} // namespace Config
} // namespace Shower

#endif // cosmic_shower_GLOBAL_H
