#ifndef PHYSICSLIST_H
#define PHYSICSLIST_H

#include "global.h"

#include <G4VModularPhysicsList.hh>

namespace Shower {
class PhysicsList : public G4VModularPhysicsList {
public:
    explicit PhysicsList(G4int ver = 1);
    ~PhysicsList() override;

    void SetCuts() override;

    void ConstructProcess() override;

    void ConstructParticle() override;
};
}

#endif // PHYSICSLIST_H
