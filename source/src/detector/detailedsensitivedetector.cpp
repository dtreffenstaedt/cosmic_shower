#include "detector/detailedsensitivedetector.h"

#include "recorder.h"

#include <G4Box.hh>
#include <G4NistManager.hh>
#include <G4PVPlacement.hh>
#include <G4SDManager.hh>
#include <G4SystemOfUnits.hh>

namespace Shower {

DetailedSensitiveDetector::DetailedSensitiveDetector(const std::string& name, const std::shared_ptr<Recorder>& recorder)
    : G4VSensitiveDetector { "/" + name }
    , m_recorder { recorder }
{
}

auto DetailedSensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory * /*ROhist*/) -> G4bool
{
    const G4Track* track = step->GetTrack();
    const G4StepPoint* pre = step->GetPreStepPoint();

    m_recorder->store_detailed_hit({ track->GetDefinition()->GetPDGEncoding(),
        track->GetMomentum().mag(),
        pre->GetMomentumDirection(),
        pre->GetPosition(),
        track->GetGlobalTime(),
        track->GetProperTime(),
        pre->GetTouchableHandle()->GetVolume(0)->GetName() });

    return true;
}
}
