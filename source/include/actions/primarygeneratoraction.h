#ifndef PRIMARYGENERATORACTION_H
#define PRIMARYGENERATORACTION_H

#include <G4ParticleGun.hh>
#include <G4SystemOfUnits.hh>
#include <G4VUserPrimaryGeneratorAction.hh>
#include <globals.hh>

#include "configmanager.h"
#include "global.h"

class G4ParticleGun;
class G4Event;
class G4Box;

namespace Shower {
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
    PrimaryGeneratorAction();
    ~PrimaryGeneratorAction() override;

    void GeneratePrimaries(G4Event* e) override;

private:
    G4ParticleGun* m_particle_gun { nullptr };
    Config::PrimaryParticle m_primary { ConfigManager::singleton()->get_primary_particle() };
    G4double m_atmosphere_height { ConfigManager::singleton()->get_atmosphere_height() * m };
    struct
    {
        G4double x;
        G4double y;
        G4double z;
    } m_offset_top {
        -m_atmosphere_height * m_primary.momentum.x / std::abs(m_primary.momentum.z) / 2,
        -m_atmosphere_height* m_primary.momentum.y / std::abs(m_primary.momentum.z) / 2,
        m_atmosphere_height / 2
    };
    //    G4Box* m_envelope_box;
};
}

#endif // PRIMARYGENERATORACTION_H
