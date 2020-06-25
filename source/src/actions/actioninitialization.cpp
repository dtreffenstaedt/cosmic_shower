#include "actions/actioninitialization.h"

#include "actions/primarygeneratoraction.h"

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
    SetUserAction(new PrimaryGeneratorAction);
}
}
