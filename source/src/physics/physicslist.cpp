#include "physics/physicslist.h"

#include <G4EmExtraPhysics.hh>
#include <G4EmStandardPhysics.hh>
#include <G4IonPhysics.hh>
#include <G4NeutronTrackingCut.hh>
#include <G4StepLimiterPhysics.hh>

#include "physics/decayconstructor.h"
#include "physics/hadronelasticconstructor.h"
#include "physics/hadroninelasticconstructor.h"
#include "physics/stoppingconstructor.h"

namespace Shower {
PhysicsList::PhysicsList(G4int ver)

{
    // EM Physics
    RegisterPhysics(new G4EmStandardPhysics { ver });

    // Synchroton Radiation & GN Physics
    RegisterPhysics(new G4EmExtraPhysics { ver });

    // Decays
    RegisterPhysics(new DecayConstructor { ver });

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
    SetCutValue(10000 * parsec, "nu_e");
    SetCutValue(10000 * parsec, "nu_tau");
    SetCutValue(10000 * parsec, "nu_mu");
    SetCutValue(10000 * parsec, "anti_nu_e");
    SetCutValue(10000 * parsec, "anti_nu_tau");
    SetCutValue(10000 * parsec, "anti_nu_mu");
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
