#ifndef PHYSICSLIST_H
#define PHYSICSLIST_H

#include "global.h"

#include <G4VModularPhysicsList.hh>

START_NAMESPACE
{
class PhysicsList : public G4VModularPhysicsList
{
public:
    PhysicsList(G4int ver = 1);
    virtual ~PhysicsList();

    virtual void SetCuts();
};
}

#endif // PHYSICSLIST_H
