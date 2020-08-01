#include "actions/steppingaction.h"

#include <G4Step.hh>
#include <G4SystemOfUnits.hh>
#include <G4Track.hh>

#include <iostream>
#include <utility>

#include "cancelcriterion.h"
#include "configuration.h"
#include "recorder.h"

namespace Shower {

SteppingAction::SteppingAction(std::shared_ptr<Recorder> recorder, std::shared_ptr<CancelCriterion> cancel_criterion, const std::shared_ptr<Configuration>& configuration)
    : m_limit { configuration->get_primary_event_limit() }
    , m_recorder { std::move(recorder) }
    , m_cancel_criterion { std::move(cancel_criterion) }
{
}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    if (m_cancel_criterion->met()) {
        G4Track* track = step->GetTrack();
        const G4StepPoint* pre = step->GetPreStepPoint();

        m_recorder->store_secondary({ pre->GetPosition(),
            track->GetMomentumDirection(),
            track->GetKineticEnergy(),
            track->GetParticleDefinition()->GetPDGEncoding() });

        track->SetTrackStatus(G4TrackStatus::fStopAndKill);
    }
    if (!m_recorder->stored_primary()) {
        const G4Track* track = step->GetTrack();
        if (track->GetParentID() == 0) // only check first secondary particle
        {
            return;
        }
        const G4ParticleDefinition* particle = track->GetParticleDefinition();
        if ((particle->GetPDGMass() + track->GetKineticEnergy()) < m_limit) {
            return;
        }
        std::cout << "Storing initial interaction\n";
        const G4StepPoint* pre = step->GetPreStepPoint();
        m_recorder->store_primary({ pre->GetPosition(), pre->GetGlobalTime() });
    }
}
}
