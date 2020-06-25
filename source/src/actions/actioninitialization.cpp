#include "actions/actioninitialization.h"

#include "actions/primarygeneratoraction.h"
#include "actions/runaction.h"
#include "actions/eventaction.h"
#include "actions/steppingaction.h"
#include "configmanager.h"

#include <G4SystemOfUnits.hh>

START_NAMESPACE
{
ActionInitialization::ActionInitialization() :
    G4VUserActionInitialization{}
{}

ActionInitialization::~ActionInitialization()
{}

void ActionInitialization::BuildForMaster() const
{
//    RunAction* runAction = new RunAction;
//    SetUserAction(runAction);
}

void ActionInitialization::Build() const
{
    SetUserAction(new PrimaryGeneratorAction);

//    RunAction* runAction = new RunAction;
//    SetUserAction(runAction);

//    EventAction* eventAction = new EventAction(runAction);
//    SetUserAction(eventAction);

//    SetUserAction(new SteppingAction(eventAction));
}
}
