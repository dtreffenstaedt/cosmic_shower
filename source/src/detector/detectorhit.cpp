#include "detector/detectorhit.h"

#include <G4UnitsTable.hh>
#include <fstream>
#include <utility>
namespace Shower {

DetectorHit::DetectorHit(
    G4ParticleDefinition* particle,
    G4double energy,
    G4VPhysicalVolume* physical_volume,
    G4ThreeVector position,
    G4ThreeVector momentum,
    G4double global_time,
    G4double proper_time)
    : m_particle { particle }
    , m_energy { energy }
    , m_physical_volume { physical_volume }
    , m_position { std::move(position) }
    , m_momentum { std::move(momentum) }
    , m_global_time { global_time }
    , m_proper_time { proper_time }
{
}

DetectorHit::DetectorHit()
    :

    m_position {}
    , m_momentum {}

{
}

DetectorHit::~DetectorHit()
    = default;

void DetectorHit::Print()
{
    std::cout << m_particle->GetPDGEncoding() << ',' << m_physical_volume->GetName() << ',' << m_position << ',' << m_momentum << ',' << m_energy << ',' << m_global_time << ',' << m_proper_time << '\n';
}

auto DetectorHit::get_pdg() -> int
{
    return m_particle->GetPDGEncoding();
}

void DetectorHit::write_to_file(std::ofstream& file)
{
    file
        << m_particle->GetPDGEncoding() << ','
        << m_position.x()<< ',' << m_position.y() << ',' << m_position.z() << ','
        << m_momentum.x() << ',' << m_momentum.y() << ',' << m_momentum.z() << ',' << m_energy
        << ',' << m_global_time << ',' << m_proper_time << '\n';
}

void DetectorHit::write_header(std::ofstream& file)
{
    file << "Particle,position_x[mm],position_y[mm],position_z[mm],momentum_x,momentum_y,momentum_z,momentum_magnitude[MeV],global_time[ns],proper_time[ns]\n";
}
}
