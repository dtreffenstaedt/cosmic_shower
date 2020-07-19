#ifndef STOPPINGCONSTRUCTOR_H
#define STOPPINGCONSTRUCTOR_H

#include "global.h"

#include <G4StoppingPhysics.hh>

namespace Shower {
class StoppingConstructor : public G4StoppingPhysics {
public:
    explicit StoppingConstructor(G4int ver = 1);
    explicit StoppingConstructor(const G4String& name, G4int ver = 1);

    ~StoppingConstructor() override;

    void ConstructParticle() override;
};
}
#endif // STOPPINGCONSTRUCTOR_H
