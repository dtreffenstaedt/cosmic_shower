#ifndef PHYSICSLIST_H
#define PHYSICSLIST_H

#include "global.h"

#include "cancelcriterion.h"
#include "recorder.h"

#include <G4VModularPhysicsList.hh>

#include <memory>

namespace Shower {
class PhysicsList : public G4VModularPhysicsList {
public:
    explicit PhysicsList(std::shared_ptr<Recorder> recorder, std::shared_ptr<CancelCriterion> cancel_criterion, const G4int ver = 1);
    ~PhysicsList() override;

    void SetCuts() override;

    void ConstructProcess() override;

    void ConstructParticle() override;
};
}

#endif // PHYSICSLIST_H
