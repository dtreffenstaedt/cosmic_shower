#ifndef STOPPINGCONSTRUCTOR_H
#define STOPPINGCONSTRUCTOR_H

#include "global.h"

#include <G4StoppingPhysics.hh>

namespace Shower
{
class StoppingConstructor : public G4StoppingPhysics
{
public:
    StoppingConstructor(G4int ver = 1);
    StoppingConstructor(const G4String& name, G4int ver = 1);

    virtual ~StoppingConstructor();

    virtual void ConstructParticle();

};
}
#endif // STOPPINGCONSTRUCTOR_H
