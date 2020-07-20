#include "actions/eventaction.h"

#include <G4Event.hh>

#include <iostream>

namespace Shower {
size_t EventAction::c_n = 1;

EventAction::EventAction()

    = default;

EventAction::~EventAction()
    = default;

void EventAction::BeginOfEventAction(const G4Event* /*anEvent*/)
{
    std::cout << "Event started\n";
#ifdef SHOWER_BENCHMARK
    m_measurement = BenchmarkManager::singleton()->start("event" + std::to_string(c_n));
#endif
    c_n++;
}

void EventAction::EndOfEventAction(const G4Event* /*anEvent*/)
{
    std::cout << "Event ended\n";
#ifdef SHOWER_BENCHMARK
    m_measurement->stop();
#endif
}
}
