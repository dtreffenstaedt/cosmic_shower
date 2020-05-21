#include "actions/primarygeneratoraction.h"

#include <G4Event.hh>
#include <G4ParticleGun.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleDefinition.hh>
#include <G4SystemOfUnits.hh>

START_NAMESPACE
{
PrimaryGeneratorAction::PrimaryGeneratorAction() :
    G4VUserPrimaryGeneratorAction(),
    m_particle_gun(0)
{
    G4int n_of_particles = 1;
    m_particle_gun = new G4ParticleGun(n_of_particles);

    G4ParticleTable* particle_table = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particle_table->FindParticle("anti_proton");
    m_particle_gun->SetParticleDefinition(particle);
    m_particle_gun->SetParticleEnergy(1*GeV);
    m_particle_gun->SetParticlePosition(G4ThreeVector(-2*m,0,0));
    m_particle_gun->SetParticleMomentumDirection(G4ThreeVector(1,0,0));
   /* 
    m_particle_gun2 = new G4ParticleGun(n_of_particles);

    G4ParticleDefinition* particle2 = particle_table->FindParticle("neutron");
    m_particle_gun2->SetParticleDefinition(particle2);
    m_particle_gun2->SetParticleEnergy(1*MeV);
    m_particle_gun2->SetParticlePosition(G4ThreeVector(-2*m,0,0));
    m_particle_gun2->SetParticleMomentumDirection(G4ThreeVector(1,0,0));*/
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete m_particle_gun;
//    delete m_particle_gun2;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* e)
{
    m_particle_gun->GeneratePrimaryVertex(e);
//    m_particle_gun2->GeneratePrimaryVertex(e);
}

}
