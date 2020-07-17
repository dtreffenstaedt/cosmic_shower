#ifndef PHYSICSLIST_H
#define PHYSICSLIST_H

#include "global.h"

#include <G4VModularPhysicsList.hh>

namespace Shower
{
class PhysicsList : public G4VModularPhysicsList
{
public:
    PhysicsList(G4int ver = 1);
    virtual ~PhysicsList();

    virtual void SetCuts() override;

    virtual void ConstructProcess() override;

    virtual void ConstructParticle() override;
};
}

#endif // PHYSICSLIST_H
