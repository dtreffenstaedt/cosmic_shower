#include "actions/primarygeneratoraction.h"

#include <G4Event.hh>
#include <G4ParticleGun.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleDefinition.hh>
#include <G4SystemOfUnits.hh>

START_NAMESPACE
{
PrimaryGeneratorAction::PrimaryGeneratorAction(const Config::PrimaryParticle& primary, const G4double &atmosphere_upper) :
    G4VUserPrimaryGeneratorAction{},
    m_particle_gun{0},
    m_primary{primary},
    m_atmosphere_upper{atmosphere_upper}
{
    G4int n_of_particles = 1;
    m_particle_gun = new G4ParticleGun(n_of_particles);

    G4ParticleTable* particle_table = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particle_table->FindParticle(primary.particle);
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
