#include "physicslist.h"

#include <G4DecayPhysics.hh>
#include <G4EmStandardPhysics.hh>
#include <G4RadioactiveDecayPhysics.hh>

START_NAMESPACE
{
PhysicsList::PhysicsList() :
    G4VModularPhysicsList{}
{
    RegisterPhysics(new G4DecayPhysics);
    RegisterPhysics(new G4EmStandardPhysics);
    RegisterPhysics(new G4RadioactiveDecayPhysics);
}

PhysicsList::~PhysicsList()
{
}

void PhysicsList::ConstructParticle()
{
}

void PhysicsList::ConstructProcess()
{

}

void PhysicsList::SetCuts()
{
    G4VUserPhysicsList::SetCuts();
}
}
