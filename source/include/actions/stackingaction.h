#ifndef STACKINGACTION_H
#define STACKINGACTION_H

#include "global.h"

#include <memory>

#include <G4UserStackingAction.hh>

namespace Shower {

class Recorder;
class CancelCriterion;

class StackingAction : public G4UserStackingAction {
public:
    StackingAction(std::shared_ptr<Recorder> recorder, std::shared_ptr<CancelCriterion> cancel_criterion);

    auto ClassifyNewTrack(const G4Track* t) -> G4ClassificationOfNewTrack override;

private:
    std::shared_ptr<Recorder> m_recorder{nullptr};
    std::shared_ptr<CancelCriterion> m_cancel_criterion{nullptr};
};
}

#endif // STACKINGACTION_H
