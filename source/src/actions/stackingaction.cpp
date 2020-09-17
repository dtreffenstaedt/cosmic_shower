#include "actions/stackingaction.h"

#include <G4Track.hh>
#include <G4NeutrinoE.hh>
#include <G4NeutrinoMu.hh>
#include <G4NeutrinoTau.hh>

#include "recorder.h"
#include "cancelcriterion.h"

namespace Shower {

StackingAction::StackingAction(std::shared_ptr<Recorder> recorder, std::shared_ptr<CancelCriterion> cancel_criterion)
    : m_recorder {recorder}
    , m_cancel_criterion {cancel_criterion}
{}

auto StackingAction::ClassifyNewTrack(const G4Track* track) -> G4ClassificationOfNewTrack {
    const auto* p = track->GetParticleDefinition();

    if (
            (p == G4NeutrinoE::NeutrinoE()) ||
            (p == G4NeutrinoMu::NeutrinoMu()) ||
            (p == G4NeutrinoTau::NeutrinoTau())
            ) {
        return fKill;
    }
    if (m_cancel_criterion->met()) {
        const G4ThreeVector pos = track->GetPosition();
        const G4ThreeVector mom = track->GetMomentumDirection();
        m_recorder->store_secondary({ { pos.x(), pos.y(), pos.z() },
            { mom.x(), mom.y(), mom.z() },
            {track->GetGlobalTime()},
            track->GetKineticEnergy(),
            track->GetParticleDefinition()->GetPDGEncoding(),
            track->GetParticleDefinition()->GetParticleName()});
        return fKill;
    }
    return fUrgent;
}
}
