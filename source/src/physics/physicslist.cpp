#include "physics/physicslist.h"

#include <G4EmStandardPhysics.hh>
#include <G4EmExtraPhysics.hh>
#include <G4HadronElasticPhysics.hh>
#include <G4HadronPhysicsFTFQGSP_BERT.hh>
#include <G4IonPhysics.hh>
#include <G4NeutronTrackingCut.hh>
#include <G4StepLimiterPhysics.hh>
#include <G4HadronicParameters.hh>

#include "physics/decayconstructor.h"
#include "physics/stoppingconstructor.h"


namespace Shower
{
PhysicsList::PhysicsList(G4int ver) :
    G4VModularPhysicsList{}
{
    // EM Physics
    RegisterPhysics( new G4EmStandardPhysics{ver} );

    // Synchroton Radiation & GN Physics
    RegisterPhysics( new G4EmExtraPhysics{ver} );

    // Decays
    RegisterPhysics( new DecayConstructor{ver} );

    // Hadron Elastic scattering
    RegisterPhysics( new G4HadronElasticPhysics{ver} );

    // Hadron Physics
    RegisterPhysics( new G4HadronPhysicsFTFQGSP_BERT{ver});

    // Stopping Physics
    RegisterPhysics( new StoppingConstructor{ver} );

    // Ion Physics
    RegisterPhysics( new G4IonPhysics{ver});

    // Neutron tracking cut
    RegisterPhysics( new G4NeutronTrackingCut{ver});

    // Introduce Tracking Cuts
    RegisterPhysics(new G4StepLimiterPhysics{ver});
}

PhysicsList::~PhysicsList()
{
}

void PhysicsList::SetCuts()
{
    SetCutsWithDefault();
}

void PhysicsList::ConstructProcess()
{
    G4VModularPhysicsList::ConstructProcess();
}

void PhysicsList::ConstructParticle()
{
    G4VModularPhysicsList::ConstructParticle();
}
}
