#include "detector/detectorhit.h"

#include <fstream>

START_NAMESPACE
{

DetectorHit::DetectorHit(
            G4ParticleDefinition* particle,
            G4double energy,
            G4VPhysicalVolume* physical_volume,
            G4ThreeVector position,
            G4ThreeVector momentum,
            G4double global_time
            ) :
    m_particle{particle},
    m_energy{energy},
    m_physical_volume{physical_volume},
    m_position{position},
    m_momentum{momentum},
    m_global_time{global_time}
{}

DetectorHit::~DetectorHit()
{
}

void DetectorHit::Print()
{
    G4cout<<m_particle->GetPDGEncoding()<<','<<m_physical_volume->GetName()<<','<<m_position<<','<<m_momentum<<','<<m_energy<<','<<m_global_time<<'\n';
}

int DetectorHit::get_pdg()
{
    return m_particle->GetPDGEncoding();
}

void DetectorHit::write_to_file(std::ofstream &file)
{
    file<<m_particle->GetPDGEncoding()<<','<<m_physical_volume->GetName()<<','<<m_position<<','<<m_momentum<<','<<m_energy<<','<<m_global_time<<'\n';
}
}