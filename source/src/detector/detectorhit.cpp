#include "detector/detectorhit.h"

#include <G4UnitsTable.hh>
#include <fstream>
#include <utility>
namespace Shower {

DetectorHit::DetectorHit(const G4double energy,
    const G4ThreeVector& position,
    const G4ThreeVector& rest_energy,
    const bool charged)
    : m_energy { energy }
    , m_position { std::move(position) }
    , m_rest_energy { std::move(rest_energy) }
    , m_charged { charged }
{
}

void DetectorHit::Print()
{
    std::cout << m_position << ',' << m_rest_energy << ',' << m_energy << ',' << m_charged << '\n';
}
}
