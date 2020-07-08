#include "actions/actioninitialization.h"

#include "actions/primarygeneratoraction.h"
#include "actions/eventaction.h"
#include "actions/steppingaction.h"

START_NAMESPACE
{
ActionInitialization::ActionInitialization() :
    G4VUserActionInitialization{}
{}

ActionInitialization::~ActionInitialization()
{}

void ActionInitialization::BuildForMaster() const
{
}

void ActionInitialization::Build() const
{
    SetUserAction(new PrimaryGeneratorAction{});

    SetUserAction(new EventAction{});

    SetUserAction(new SteppingAction{});
}
}
