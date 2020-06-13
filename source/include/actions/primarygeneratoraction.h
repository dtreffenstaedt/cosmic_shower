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

START_NAMESPACE
{
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    PrimaryGeneratorAction(const PrimaryParticle& primary);
    virtual ~PrimaryGeneratorAction();

    virtual void GeneratePrimaries(G4Event* e);


private:
    G4ParticleGun* m_particle_gun;
    PrimaryParticle m_primary;
//    G4Box* m_envelope_box;
};
}

#endif // PRIMARYGENERATORACTION_H
