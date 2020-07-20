#include "actions/actioninitialization.h"

#include "actions/eventaction.h"
#include "actions/primarygeneratoraction.h"
#include "actions/steppingaction.h"

namespace Shower {
ActionInitialization::ActionInitialization()

    = default;

ActionInitialization::~ActionInitialization()
    = default;

void ActionInitialization::BuildForMaster() const
{
}

void ActionInitialization::Build() const
{
    SetUserAction(new PrimaryGeneratorAction {});

    SetUserAction(new EventAction {});

    SetUserAction(new SteppingAction {});
}
}
