#ifndef DETECTORHIT_H
#define DETECTORHIT_H

#include "global.h"

#include <G4ParticleDefinition.hh>
#include <G4ThreeVector.hh>
#include <G4VHit.hh>
#include <G4VPhysicalVolume.hh>

namespace Shower {

class DetectorHit : public G4VHit {
public:
    explicit DetectorHit(
        const G4double energy,
        const G4ThreeVector& position,
        const G4ThreeVector& rest_energy,
        const bool charged);

    explicit DetectorHit() = default;

    ~DetectorHit() override = default;

    // +++ reimplemented from G4VHit
    void Print() override;
    // --- reimplemented from G4VHit

private:
    G4double m_energy {};
    G4ThreeVector m_position {};
    G4ThreeVector m_rest_energy {};
    bool m_charged {};
};

}
#endif // DETECTORHIT_H
