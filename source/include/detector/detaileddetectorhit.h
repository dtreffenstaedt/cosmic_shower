#ifndef DETAILEDDETECTORHIT_H
#define DETAILEDDETECTORHIT_H

#include "global.h"

#include <G4ParticleDefinition.hh>
#include <G4ThreeVector.hh>
#include <G4VHit.hh>
#include <G4VPhysicalVolume.hh>

namespace Shower {

class DetailedDetectorHit : public G4VHit {
public:
    DetailedDetectorHit(
        const G4int particle,
        const G4double momentum,
        const G4ThreeVector& position,
        const G4ThreeVector& momentum_direction,
        const G4double global_time,
        const G4double proper_time,
        const G4String& name);

    DetailedDetectorHit();

    ~DetailedDetectorHit() override = default;

    // +++ reimplemented from G4VHit
    void Print() override;
    // --- reimplemented from G4VHit

    void write_to_file(std::ofstream& file);

    static void write_header(std::ofstream& file);

    auto get_pdg() -> int;

private:
    G4int m_particle {};
    G4double m_momentum {};
    G4ThreeVector m_position {};
    G4ThreeVector m_momentum_direction {};
    G4double m_global_time {};
    G4double m_proper_time {};
    G4String m_name {};
};

}
#endif // DETAILEDDETECTORHIT_H
