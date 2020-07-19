#ifndef DECAYCONSTRUCTOR_H
#define DECAYCONSTRUCTOR_H

#include "global.h"

#include <G4DecayPhysics.hh>

namespace Shower {
class DecayConstructor : public G4DecayPhysics {
public:
    explicit DecayConstructor(G4int ver = 1);
    explicit DecayConstructor(const G4String& name, G4int ver = 1);

    ~DecayConstructor() override;

    void ConstructParticle() override;
};
}
#endif // DECAYCONSTRUCTOR_H
