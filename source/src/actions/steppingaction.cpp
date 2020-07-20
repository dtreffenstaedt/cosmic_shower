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
    RecorderManager::singleton()->store_primary(track->GetStep()->GetPreStepPoint()->GetPosition(), track->GetStep()->GetPreStepPoint()->GetGlobalTime());
}
}
