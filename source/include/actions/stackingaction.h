#ifndef STACKINGACTION_H
#define STACKINGACTION_H

#include "global.h"


#include <G4UserStackingAction.hh>

namespace Shower {


class StackingAction : public G4UserStackingAction {
public:
    StackingAction();

    auto ClassifyNewTrack(const G4Track* t) -> G4ClassificationOfNewTrack override;
};
}

#endif // STACKINGACTION_H
