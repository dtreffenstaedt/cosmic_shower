#ifndef DETECTORHIT_H
#define DETECTORHIT_H

#include "global.h"

#include <G4VHit.hh>
#include <G4ParticleDefinition.hh>
#include <G4VPhysicalVolume.hh>
#include <G4ThreeVector.hh>

namespace Shower
{

class DetectorHit : public G4VHit
{
public:
    DetectorHit(G4ParticleDefinition* particle,
                G4double energy,
                G4VPhysicalVolume* physical_volume,
                const G4ThreeVector &position,
                const G4ThreeVector &momentum,
                G4double global_time,
                G4double proper_time
                );

    DetectorHit();

    virtual ~DetectorHit();

    // +++ reimplemented from G4VHit
    virtual void Print();
    // --- reimplemented from G4VHit

    void write_to_file(std::ofstream& file);

    void write_header(std::ofstream& file);

    int get_pdg();

private:
    G4ParticleDefinition* m_particle;
    G4double m_energy;
    G4VPhysicalVolume* m_physical_volume;
    G4ThreeVector m_position;
    G4ThreeVector m_momentum;
    G4double m_global_time;
    G4double m_proper_time;
};

}
#endif // DETECTORHIT_H
