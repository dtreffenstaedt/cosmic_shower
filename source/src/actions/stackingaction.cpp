#include "actions/stackingaction.h"

#include <G4Track.hh>
#include <G4NeutrinoE.hh>
#include <G4NeutrinoMu.hh>
#include <G4NeutrinoTau.hh>


namespace Shower {

StackingAction::StackingAction() = default;

auto StackingAction::ClassifyNewTrack(const G4Track* t) -> G4ClassificationOfNewTrack {
    const auto* p = t->GetParticleDefinition();

    if (
            (p == G4NeutrinoE::NeutrinoE()) ||
            (p == G4NeutrinoMu::NeutrinoMu()) ||
            (p == G4NeutrinoTau::NeutrinoTau())
            ) {
        return fKill;
    }
    return fUrgent;
}
}
