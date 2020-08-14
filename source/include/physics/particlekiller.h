#ifndef PARTICLEKILLER_H
#define PARTICLEKILLER_H

#include "cancelcriterion.h"
#include "global.h"
#include "recorder.h"

#include <G4VDiscreteProcess.hh>
#include <G4VPhysicsConstructor.hh>

#include <memory>

namespace Shower {

class ParticleKiller : public G4VDiscreteProcess {
public:
    ParticleKiller(std::shared_ptr<Recorder> recorder, std::shared_ptr<CancelCriterion> cancel_criterion);

    auto IsApplicable(const G4ParticleDefinition & /*particle*/) -> G4bool override;
    auto PostStepDoIt(const G4Track& track, const G4Step& step) -> G4VParticleChange* override;
    auto GetMeanFreePath(const G4Track& /*track*/, G4double /*step_size*/, G4ForceCondition * /*condition*/) -> G4double override;
    auto PostStepGetPhysicalInteractionLength(const G4Track& /*track*/, G4double /*step_size*/, G4ForceCondition* condition) -> G4double override;

private:
    std::shared_ptr<Recorder> m_recorder;
    std::shared_ptr<CancelCriterion> m_cancel_criterion;
};

class ParticleKillerConstructor : public G4VPhysicsConstructor {
public:
    ParticleKillerConstructor(std::shared_ptr<Recorder> recorder, std::shared_ptr<CancelCriterion> cancel_criterion, const G4int ver = 0);

    void ConstructParticle() override;
    void ConstructProcess() override;

private:
    std::shared_ptr<Recorder> m_recorder;
    std::shared_ptr<CancelCriterion> m_cancel_criterion;
};

}

#endif // PARTICLEKILLER_H
