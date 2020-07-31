#ifndef PRIMARYGENERATORACTION_H
#define PRIMARYGENERATORACTION_H

#include <G4ParticleGun.hh>
#include <G4SystemOfUnits.hh>
#include <G4VUserPrimaryGeneratorAction.hh>
#include <globals.hh>

#include "configuration.h"
#include "global.h"

class G4ParticleGun;
class G4Event;
class G4Box;

namespace Shower {
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
    PrimaryGeneratorAction(const std::shared_ptr<Configuration>& configuration);
    ~PrimaryGeneratorAction() override;

    void GeneratePrimaries(G4Event* e) override;

private:
    std::vector<G4ParticleGun*> m_particle_guns {};
    G4double m_atmosphere_height { 0 * m };
    std::vector<Config::PrimaryParticle> m_primaries {};
    struct
    {
        G4double x { 0 * m };
        G4double y { 0 * m };
        G4double z { 0 * m };
    } m_offset_top {};
    //    G4Box* m_envelope_box;
};
}

#endif // PRIMARYGENERATORACTION_H
