#include "detector/sensitivedetector.h"

#include "recorder.h"

#include <G4Box.hh>
#include <G4NistManager.hh>
#include <G4PVPlacement.hh>
#include <G4SDManager.hh>
#include <G4SystemOfUnits.hh>
#include <utility>

namespace Shower {

SensitiveDetector::SensitiveDetector(const std::string& name, std::shared_ptr<Recorder> recorder)
    : G4VSensitiveDetector { "/" + name }
    , m_recorder { std::move(recorder) }
{
}

auto SensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory * /*ROhist*/) -> G4bool
{
    G4Track* track = step->GetTrack();
    const G4ParticleDefinition* particle = track->GetDefinition();

    m_recorder->store_ground_intensity({ step->GetPreStepPoint()->GetPosition(), particle->GetPDGMass(), track->GetKineticEnergy(), std::abs(particle->GetPDGCharge()) <= std::numeric_limits<G4double>::epsilon() });

    track->SetTrackStatus(fStopAndKill);

    return true;
}
}
