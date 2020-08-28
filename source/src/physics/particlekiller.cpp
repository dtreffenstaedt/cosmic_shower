#include "physics/particlekiller.h"

#include <G4HadronicProcessStore.hh>
#include <G4ProcessManager.hh>
#include <utility>

namespace Shower {

ParticleKiller::ParticleKiller(std::shared_ptr<Recorder> recorder, std::shared_ptr<CancelCriterion> cancel_criterion)
    : G4VDiscreteProcess("particleKiller", fUserDefined)
    , m_recorder { std::move(recorder) }
    , m_cancel_criterion { std::move(cancel_criterion) }
{
}

auto ParticleKiller::IsApplicable(const G4ParticleDefinition & /*particle*/) -> G4bool
{
    return true;
}

auto ParticleKiller::PostStepDoIt(const G4Track& track, const G4Step& step) -> G4VParticleChange*
{
    const int pdg = std::abs(track.GetParticleDefinition()->GetPDGEncoding());
    if (!((pdg == 12) || (pdg == 14) || (pdg == 16))) {
        const G4StepPoint* pre = step.GetPreStepPoint();
        const G4ThreeVector pos = pre->GetPosition();
        const G4ThreeVector mom = track.GetMomentumDirection();
        m_recorder->store_secondary({ { pos.x(), pos.y(), pos.z() },
            { mom.x(), mom.y(), mom.z() },
            track.GetKineticEnergy(),
            track.GetParticleDefinition()->GetPDGEncoding(),
            track.GetParticleDefinition()->GetParticleName() });
    }

    pParticleChange->Initialize(track);
    pParticleChange->ProposeTrackStatus(fStopAndKill);
    return pParticleChange;
}

auto ParticleKiller::GetMeanFreePath(const G4Track& /*track*/, G4double /*step_size*/, G4ForceCondition * /*condition*/) -> G4double
{
    return std::numeric_limits<G4double>::max();
}

auto ParticleKiller::PostStepGetPhysicalInteractionLength(const G4Track& track, G4double /*step_size*/, G4ForceCondition* condition) -> G4double
{
    *condition = NotForced;

    const int pdg = std::abs(track.GetParticleDefinition()->GetPDGEncoding());

    if (m_cancel_criterion->met() || (pdg == 12) || (pdg == 14) || (pdg == 16)) {
        return 0.0;
    }
    return std::numeric_limits<G4double>::max();
}

ParticleKillerConstructor::ParticleKillerConstructor(std::shared_ptr<Recorder> recorder, std::shared_ptr<CancelCriterion> cancel_criterion, const G4int ver)
    : G4VPhysicsConstructor { "ParticleKillerProcess", ver }
    , m_recorder { std::move(recorder) }
    , m_cancel_criterion { std::move(cancel_criterion) }
{
}

void ParticleKillerConstructor::ConstructParticle()
{
}

void ParticleKillerConstructor::ConstructProcess()
{
    auto* process = new ParticleKiller { m_recorder, m_cancel_criterion };
    auto* particles = GetParticleIterator();
    particles->reset();

    while ((*particles)()) {
        auto* particle = particles->value();
        particle->GetProcessManager()->AddDiscreteProcess(process);
    }
}

}
