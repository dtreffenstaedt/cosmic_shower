#include "actioninitialization.h"

#include "actions/primarygeneratoraction.h"
#include "actions/runaction.h"
#include "actions/eventaction.h"
#include "actions/steppingaction.h"

#include <G4SystemOfUnits.hh>

START_NAMESPACE
{
ActionInitialization::ActionInitialization(const Config::PrimaryParticle& primary, const double &atmosphere_upper) :
    G4VUserActionInitialization{},
    m_primary{primary},
    m_atmosphere_upper{atmosphere_upper * m}
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
    SetUserAction(new PrimaryGeneratorAction(m_primary, m_atmosphere_upper));

    RunAction* runAction = new RunAction;
    SetUserAction(runAction);

    EventAction* eventAction = new EventAction(runAction);
    SetUserAction(eventAction);

    SetUserAction(new SteppingAction(eventAction));
}
}
