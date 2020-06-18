#ifndef PHYSICSLIST_H
#define PHYSICSLIST_H

#include "global.h"

#include <G4VModularPhysicsList.hh>


START_NAMESPACE
{
class PhysicsList : public G4VModularPhysicsList
{
public:
    PhysicsList();
    virtual ~PhysicsList();

    virtual void ConstructParticle();
    virtual void ConstructProcess();

    virtual void SetCuts();
};
}

#endif // PHYSICSLIST_H
