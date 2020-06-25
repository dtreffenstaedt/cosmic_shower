#include "physicslist.h"

#include <G4DecayPhysics.hh>
#include <G4EmStandardPhysics.hh>
#include <G4RadioactiveDecayPhysics.hh>

#include <G4Geantino.hh>
#include <G4ChargedGeantino.hh>
#include <G4Gamma.hh>
#include <G4Electron.hh>
#include <G4Positron.hh>
#include <G4Proton.hh>
#include <G4AntiProton.hh>
#include <G4Neutron.hh>
#include <G4AntiNeutron.hh>
#include <G4MuonMinus.hh>
#include <G4MuonPlus.hh>
#include <G4PionPlus.hh>
#include <G4PionMinus.hh>

START_NAMESPACE
{
PhysicsList::PhysicsList() :
    G4VUserPhysicsList{}
{}

PhysicsList::~PhysicsList()
{
}

void PhysicsList::ConstructParticle()
{


}

void PhysicsList::ConstructProcess()
{
    AddTransportation();
}

void PhysicsList::SetCuts()
{
    G4VUserPhysicsList::SetCuts();
}

void PhysicsList::construct_bosons()
{
    // +++ pseudo-particle used by Geant4 internally
    G4Geantino::GeantinoDefinition();
    G4ChargedGeantino::ChargedGeantinoDefinition();
    // --- pseudo-particle used by Geant4 internally


    G4Gamma::GammaDefinition();
}

void PhysicsList::construct_leptons()
{

    G4Electron::ElectronDefinition();
    G4Positron::PositronDefinition();

    G4MuonMinus::MuonMinusDefinition();
    G4MuonPlus::MuonPlusDefinition();

}

void PhysicsList::construct_mesons()
{

}

void PhysicsList::construct_baryons()
{
    G4Proton::ProtonDefinition();
    G4AntiProton::AntiProtonDefinition();
    G4Neutron::NeutronDefinition();
    G4AntiNeutron::AntiNeutronDefinition();
}
}
