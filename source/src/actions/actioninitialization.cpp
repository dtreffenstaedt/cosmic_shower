#include "actions/actioninitialization.h"

#include "actions/eventaction.h"
#include "actions/primarygeneratoraction.h"
#include "actions/steppingaction.h"

namespace Shower {
ActionInitialization::ActionInitialization(const std::shared_ptr<Recorder>& recorder, const std::shared_ptr<Configuration>& configuration)
    : m_recorder { recorder }
    , m_configuration { configuration }
{
}

void ActionInitialization::BuildForMaster() const
{
}

void ActionInitialization::Build() const
{
    SetUserAction(new PrimaryGeneratorAction { m_configuration });

    SetUserAction(new EventAction {});

    SetUserAction(new SteppingAction { m_recorder, m_configuration });
}
}
