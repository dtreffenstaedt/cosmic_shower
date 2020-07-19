#include "physics/physicslist.h"

#include <G4EmExtraPhysics.hh>
#include <G4EmStandardPhysics.hh>
#include <G4HadronElasticPhysics.hh>
#include <G4HadronPhysicsFTFQGSP_BERT.hh>
#include <G4HadronicParameters.hh>
#include <G4IonPhysics.hh>
#include <G4NeutronTrackingCut.hh>
#include <G4StepLimiterPhysics.hh>

#include "physics/decayconstructor.h"
#include "physics/hadronelasticconstructor.h"
#include "physics/hadroninelasticconstructor.h"
#include "physics/stoppingconstructor.h"

namespace Shower {
PhysicsList::PhysicsList(G4int ver)
    : G4VModularPhysicsList {}
{
    // EM Physics
    RegisterPhysics(new G4EmStandardPhysics { ver });

    // Synchroton Radiation & GN Physics
    RegisterPhysics(new G4EmExtraPhysics { ver });

    // Decays
    RegisterPhysics(new DecayConstructor { ver });

    // Hadron Elastic scattering
    //    RegisterPhysics(new G4HadronElasticPhysics { ver });

    // Hadron Physics
    //    RegisterPhysics(new G4HadronPhysicsFTFQGSP_BERT { ver });

    // Stopping Physics
    RegisterPhysics(new StoppingConstructor { ver });

    // Ion Physics
    RegisterPhysics(new G4IonPhysics { ver });

    // Neutron tracking cut
    RegisterPhysics(new G4NeutronTrackingCut { ver });

    // Introduce Tracking Cuts
    RegisterPhysics(new G4StepLimiterPhysics { ver });

    // Very High Energy Physics (100 TeV < E < 10^20 eV)
    RegisterPhysics(new HadronElasticConstructor { ver });
    RegisterPhysics(new HadronInelasticConstructor { ver });
}

PhysicsList::~PhysicsList()
    = default;

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
