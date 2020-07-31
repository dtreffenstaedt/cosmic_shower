#include "detector/detaileddetectorhit.h"

#include <G4UnitsTable.hh>
#include <fstream>
#include <utility>
namespace Shower {

DetailedDetectorHit::DetailedDetectorHit(
    const G4int particle,
    const G4double momentum,
    const G4ThreeVector& position,
    const G4ThreeVector& momentum_direction,
    const G4double global_time,
    const G4double proper_time,
    const G4String& name)
    : m_particle { particle }
    , m_momentum { momentum }
    , m_position { std::move(position) }
    , m_momentum_direction { std::move(momentum_direction) }
    , m_global_time { global_time }
    , m_proper_time { proper_time }
    , m_name { name }
{
}

DetailedDetectorHit::DetailedDetectorHit()
    :

    m_position {}
    , m_momentum_direction {}

{
}

void DetailedDetectorHit::Print()
{
    std::cout << m_particle << ',' << m_name << ',' << m_position << ',' << m_momentum_direction << ',' << m_momentum << ',' << m_global_time << ',' << m_proper_time << '\n';
}

auto DetailedDetectorHit::get_pdg() -> int
{
    return m_particle;
}

void DetailedDetectorHit::write_to_file(std::ofstream& file)
{
    file
        << m_particle << ','
        << m_name << ','
        << m_position.x() << ',' << m_position.y() << ',' << m_position.z() << ','
        << m_momentum_direction.x() << ',' << m_momentum_direction.y() << ',' << m_momentum_direction.z() << ',' << m_momentum
        << ',' << m_global_time << ',' << m_proper_time << '\n';
}

void DetailedDetectorHit::write_header(std::ofstream& file)
{
    file << "Particle,Detector,position_x[mm],position_y[mm],position_z[mm],momentum_x,momentum_y,momentum_z,momentum_magnitude[MeV],global_time[ns],proper_time[ns]\n";
}
}
