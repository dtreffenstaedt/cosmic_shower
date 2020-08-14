#include "physics/physicslist.h"

#include <G4EmExtraPhysics.hh>
#include <G4EmStandardPhysics.hh>
#include <G4IonPhysics.hh>
#include <G4NeutronTrackingCut.hh>
#include <G4StepLimiterPhysics.hh>
#include <G4HadronPhysicsFTFQGSP_BERT.hh>
#include <G4HadronElasticPhysics.hh>

#include "physics/decayconstructor.h"
#include "physics/hadronelasticconstructor.h"
#include "physics/hadroninelasticconstructor.h"
#include "physics/stoppingconstructor.h"
#include "physics/particlekiller.h"

namespace Shower {
PhysicsList::PhysicsList(std::shared_ptr<Recorder> recorder, std::shared_ptr<CancelCriterion> cancel_criterion, const G4int ver)

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
    RegisterPhysics(new G4StepLimiterPhysics { static_cast<char>(ver) });

    RegisterPhysics(new G4HadronPhysicsFTFQGSP_BERT { ver });
    RegisterPhysics(new G4HadronElasticPhysics { ver });

//    RegisterPhysics(new HadronElasticConstructor { ver });
//    RegisterPhysics(new HadronInelasticConstructor { ver });

    RegisterPhysics(new ParticleKillerConstructor { recorder, cancel_criterion, ver });

    std::cout<<"Geantino pdg: "<<G4Geantino::GeantinoDefinition()->GetPDGEncoding()<<'\n'<<std::flush;
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
