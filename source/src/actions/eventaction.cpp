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
    m_start = std::chrono::steady_clock::now();
}

void EventAction::EndOfEventAction(const G4Event* /*anEvent*/)
{
    std::chrono::steady_clock::duration d = std::chrono::steady_clock::now() - m_start;
    std::cout << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(d).count() << "ms\n";
    std::cout << "Event ended\n";
#ifdef SHOWER_BENCHMARK
    m_measurement->stop();
#endif
}
}
