#include "detector/sensitivedetector.h"

#include "configuration.h"
#include "parameters.h"
#include "recorder.h"

#include <G4Box.hh>
#include <G4NistManager.hh>
#include <G4PVPlacement.hh>
#include <G4SDManager.hh>
#include <G4SystemOfUnits.hh>

namespace Shower {

SensitiveDetector::SensitiveDetector(const std::string& name, const std::shared_ptr<Recorder>& recorder)
    : G4VSensitiveDetector { "/" + name }
    , m_recorder { recorder }
{
}

auto SensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory * /*ROhist*/) -> G4bool
{
    const G4Track* track = step->GetTrack();
    const G4ParticleDefinition* particle = track->GetDefinition();

    m_recorder->store_ground_intensity({ step->GetPreStepPoint()->GetPosition(), particle->GetPDGMass(), track->GetKineticEnergy(), particle->GetPDGCharge() <= std::numeric_limits<G4double>::epsilon() });

    return true;
}
}
