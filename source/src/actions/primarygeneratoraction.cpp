#include "actions/primarygeneratoraction.h"

#include <G4Event.hh>
#include <G4ParticleGun.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleDefinition.hh>
#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>

START_NAMESPACE
{
PrimaryGeneratorAction::PrimaryGeneratorAction() :
    G4VUserPrimaryGeneratorAction{},
    m_particle_gun{0},
    m_primary{ConfigManager::singleton()->get_primary_particle()},
    m_atmosphere_upper{ConfigManager::singleton()->get_atmosphere_height() * m}
{
    m_particle_gun = new G4ParticleGun(m_primary.n_particles);

    std::cout<<"setting up primary generator: \n\tE_kin = "<<G4BestUnit(m_primary.momentum.m * MeV, "Energy")<<"\n\tn = "<<std::to_string(m_primary.n_particles)<<'\n';
    G4ParticleTable* particle_table = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particle_table->FindParticle(m_primary.particle);
    m_particle_gun->SetParticleDefinition(particle);
    m_particle_gun->SetParticleEnergy(m_primary.momentum.m * MeV);
    m_particle_gun->SetParticlePosition(G4ThreeVector(m_primary.origin.x * m, m_primary.origin.y * m, (m_atmosphere_upper * 0.5) + (m_primary.origin.z * m)));
    m_particle_gun->SetParticleMomentumDirection(G4ThreeVector(m_primary.momentum.x, m_primary.momentum.y, m_primary.momentum.z));
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete m_particle_gun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* e)
{
    m_particle_gun->GeneratePrimaryVertex(e);
}

}
