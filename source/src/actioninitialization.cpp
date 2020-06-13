#include "actioninitialization.h"

#include "actions/primarygeneratoraction.h"
#include "actions/runaction.h"
#include "actions/eventaction.h"
#include "actions/steppingaction.h"


START_NAMESPACE
{
ActionInitialization::ActionInitialization(const Config::PrimaryParticle& primary) :
    G4VUserActionInitialization{},
    m_primary{primary}
{}

ActionInitialization::~ActionInitialization()
{}

void ActionInitialization::BuildForMaster() const
{
    RunAction* runAction = new RunAction;
    SetUserAction(runAction);
}

void ActionInitialization::Build() const
{
    SetUserAction(new PrimaryGeneratorAction(m_primary));

    RunAction* runAction = new RunAction;
    SetUserAction(runAction);

    EventAction* eventAction = new EventAction(runAction);
    SetUserAction(eventAction);

    SetUserAction(new SteppingAction(eventAction));
}
}
