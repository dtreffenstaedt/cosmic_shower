#include "actions/eventaction.h"

#include <G4Event.hh>

#include <iostream>

START_NAMESPACE
{
size_t EventAction::c_n = 1;

EventAction::EventAction() :
    G4UserEventAction{}
{
}

EventAction::~EventAction()
{
}

void EventAction::BeginOfEventAction(const G4Event*)
{
    std::cout<<"Event started\n";
#ifdef SHOWER_BENCHMARK
    m_measurement = TimeManager::singleton()->start("event" + std::to_string(c_n));
#endif
    c_n++;
}

void EventAction::EndOfEventAction(const G4Event*)
{
    std::cout<<"Event ended\n";
#ifdef SHOWER_BENCHMARK
    m_measurement->stop();
#endif
}
}
