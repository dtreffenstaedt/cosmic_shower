#include "physics/stoppingconstructor.h"

#include <G4Electron.hh>
#include <G4MuonMinus.hh>
#include <G4MuonPlus.hh>
#include <G4Positron.hh>
#include <G4TauMinus.hh>
#include <G4TauPlus.hh>

#include <G4BaryonConstructor.hh>
#include <G4BosonConstructor.hh>
#include <G4IonConstructor.hh>
#include <G4MesonConstructor.hh>
#include <G4ShortLivedConstructor.hh>

namespace Shower {
StoppingConstructor::StoppingConstructor(G4int ver)
    : G4StoppingPhysics { ver }
{
}

StoppingConstructor::StoppingConstructor(const G4String& name, G4int ver)
    : G4StoppingPhysics { name, ver }
{
}

StoppingConstructor::~StoppingConstructor()
{
    G4StoppingPhysics::~G4StoppingPhysics();
}

void StoppingConstructor::ConstructParticle()
{
    // +++ Leptons, without neutrinos
    G4Electron::ElectronDefinition();
    G4Positron::PositronDefinition();

    G4MuonMinus::MuonMinusDefinition();
    G4MuonPlus::MuonPlusDefinition();

    G4TauMinus::TauMinusDefinition();
    G4TauPlus::TauPlusDefinition();
    // --- Leptons, without neutrinos

    G4MesonConstructor mesons;
    mesons.ConstructParticle();

    G4BaryonConstructor baryons;
    baryons.ConstructParticle();
}
}
#include "physics/stoppingconstructor.h"
