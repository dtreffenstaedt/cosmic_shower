#ifndef DECAYCONSTRUCTOR_H
#define DECAYCONSTRUCTOR_H


#include "global.h"

#include <G4DecayPhysics.hh>

namespace Shower
{
class DecayConstructor : public G4DecayPhysics
{
public:
    DecayConstructor(G4int ver = 1);
    DecayConstructor(const G4String& name, G4int ver = 1);

    virtual ~DecayConstructor();

    virtual void ConstructParticle();

};
}
#endif // DECAYCONSTRUCTOR_H
