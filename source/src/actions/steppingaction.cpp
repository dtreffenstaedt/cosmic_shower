#include "actions/steppingaction.h"

#include "configmanager.h"
#include "recordermanager.h"

#include <G4Step.hh>
#include <G4SystemOfUnits.hh>
#include <G4Track.hh>

#include <iostream>

namespace Shower {

SteppingAction::SteppingAction()

    = default;

SteppingAction::~SteppingAction()
    = default;

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    G4Track* track = step->GetTrack();
    const G4ParticleDefinition* particle = track->GetParticleDefinition();
    const G4StepPoint* pre = step->GetPreStepPoint();
    if (pre->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName() == "Intensitycatcher") {
        RecorderManager::singleton()->store_momentum(pre->GetPosition(), particle->GetPDGMass(), track->GetKineticEnergy(), (particle->GetPDGCharge()) > std::numeric_limits<double>::epsilon() );
        track->SetTrackStatus(G4TrackStatus::fStopAndKill);
        return;
    }
    if (!RecorderManager::singleton()->stored_primary()) {
        if (track->GetParentID() == 0) // only check first secondary particle
        {
            return;
        }
        if ((particle->GetPDGMass() + track->GetKineticEnergy()) < m_limit) {
            return;
        }

        std::cout << "Storing initial interaction\n";
        RecorderManager::singleton()->store_primary(pre->GetPosition(), pre->GetGlobalTime());
    }
}
}
