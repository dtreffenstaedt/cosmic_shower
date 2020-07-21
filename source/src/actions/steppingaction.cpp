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

    if (step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName() == "Intensitycatcher") {
        G4Track* track = step->GetTrack();
        RecorderManager::singleton()->store_momentum(step->GetPreStepPoint()->GetPosition(), track->GetMomentum().mag(), track->GetKineticEnergy(), (track->GetParticleDefinition()->GetPDGCharge()) > std::numeric_limits<double>::epsilon() );
        track->SetTrackStatus(G4TrackStatus::fStopAndKill);
        return;
    }
    if (RecorderManager::singleton()->stored_primary()) {
        return;
    }
    G4Track* track = step->GetTrack();
    if (track->GetParentID() == 0) // only check first secondary particle
    {
        return;
    }
    if ((track->GetParticleDefinition()->GetPDGMass() + track->GetKineticEnergy()) < m_limit) {
        return;
    }

    std::cout << "Storing initial interaction\n";
    RecorderManager::singleton()->store_primary(step->GetPreStepPoint()->GetPosition(), step->GetPreStepPoint()->GetGlobalTime());
}
}
