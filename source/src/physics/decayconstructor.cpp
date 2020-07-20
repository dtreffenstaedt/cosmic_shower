#include "physics/decayconstructor.h"

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
DecayConstructor::DecayConstructor(G4int ver)
    : G4DecayPhysics { ver }
{
}

DecayConstructor::DecayConstructor(const G4String& name, G4int ver)
    : G4DecayPhysics { name, ver }
{
}

DecayConstructor::~DecayConstructor()
{
    G4DecayPhysics::~G4DecayPhysics();
}

void DecayConstructor::ConstructParticle()
{
    G4BosonConstructor bosons;
    G4BosonConstructor::ConstructParticle();

    // +++ Leptons, without neutrinos
    G4Electron::ElectronDefinition();
    G4Positron::PositronDefinition();

    G4MuonMinus::MuonMinusDefinition();
    G4MuonPlus::MuonPlusDefinition();

    G4TauMinus::TauMinusDefinition();
    G4TauPlus::TauPlusDefinition();
    // --- Leptons, without neutrinos

    G4MesonConstructor mesons;
    G4MesonConstructor::ConstructParticle();

    G4BaryonConstructor baryons;
    G4BaryonConstructor::ConstructParticle();

    G4IonConstructor ions;
    G4IonConstructor::ConstructParticle();

    G4ShortLivedConstructor short_lived;
    G4ShortLivedConstructor::ConstructParticle();
}
}
