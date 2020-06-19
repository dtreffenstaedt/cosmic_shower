#ifndef PHYSICSLIST_H
#define PHYSICSLIST_H

#include "global.h"

#include <G4VUserPhysicsList.hh>


START_NAMESPACE
{
class PhysicsList : public G4VUserPhysicsList
{
public:
    PhysicsList();
    virtual ~PhysicsList();

    virtual void ConstructParticle();
    virtual void ConstructProcess();

    virtual void SetCuts();

private:
    // +++ inidividual construction methods
    void construct_bosons();
    void construct_leptons();
    void construct_mesons();
    void construct_baryons();
    // --- inidividual construction methods
};
}

#endif // PHYSICSLIST_H
