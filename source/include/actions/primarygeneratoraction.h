#ifndef PRIMARYGENERATORACTION_H
#define PRIMARYGENERATORACTION_H

#include <G4VUserPrimaryGeneratorAction.hh>
#include <G4ParticleGun.hh>
#include <globals.hh>

#include "global.h"
#include "configmanager.h"

class G4ParticleGun;
class G4Event;
class G4Box;

namespace Shower
{
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    PrimaryGeneratorAction();
    virtual ~PrimaryGeneratorAction();

    virtual void GeneratePrimaries(G4Event* e);


private:
    G4ParticleGun* m_particle_gun;
    Config::PrimaryParticle m_primary;
    G4double m_atmosphere_height;
    struct
    {
        G4double x;
        G4double y;
        G4double z;
    } m_offset_top;
//    G4Box* m_envelope_box;
};
}

#endif // PRIMARYGENERATORACTION_H
