#include "actions/primarygeneratoraction.h"

#include "recorder.h"

#include <G4Event.hh>
#include <G4ParticleDefinition.hh>
#include <G4ParticleGun.hh>
#include <G4ParticleTable.hh>
#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>

namespace Shower {
PrimaryGeneratorAction::PrimaryGeneratorAction(const std::shared_ptr<Recorder>& recorder, const std::shared_ptr<Configuration>& configuration)
    : m_atmosphere_height { configuration->get_atmosphere_height() }
    , m_primaries { configuration->get_primaries() }
    , m_offset_top { 0, 0, m_atmosphere_height * 0.5 }
{
    G4ParticleTable* particle_table = G4ParticleTable::GetParticleTable();
    for (auto& primary : m_primaries) {
        G4ParticleDefinition* particle = particle_table->FindParticle(primary.particle);
        if (particle == nullptr) {
            particle = particle_table->FindParticle(primary.name);
            if (particle == nullptr) {
                std::cerr << "Error: could not find particle " << primary.particle << '(' << primary.name << ')' << '\n'
                          << std::flush;
                continue;
            }
        }
        auto* gun = new G4ParticleGun(primary.n_particles);
        gun->SetParticleDefinition(particle);
        gun->SetParticleEnergy(primary.momentum.m);
        if (primary.origin.absolute) {
            gun->SetParticlePosition(G4ThreeVector(primary.origin.x, primary.origin.y, primary.origin.z));
        } else {
            gun->SetParticlePosition(G4ThreeVector((primary.origin.x) + m_offset_top.x, (primary.origin.y) + m_offset_top.y, (primary.origin.z) + m_offset_top.z));
        }
        gun->SetParticleMomentumDirection(G4ThreeVector(primary.momentum.x, primary.momentum.y, primary.momentum.z));
        gun->SetParticleTime(primary.time.global);
        m_particle_guns.push_back(gun);
        recorder->store_primary_energy(primary.momentum.m);
    }
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    for (auto* gun : m_particle_guns) {
        delete gun;
    }
    m_particle_guns.clear();
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* e)
{
    for (auto* gun : m_particle_guns) {
        gun->GeneratePrimaryVertex(e);
    }
}

}
