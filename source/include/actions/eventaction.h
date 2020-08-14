#ifndef EVENTACTION_H
#define EVENTACTION_H

#include "global.h"

#ifdef SHOWER_BENCHMARK
#include "benchmark.h"
#endif
#include "recorder.h"

#include <G4UserEventAction.hh>
#include <cstddef>

class G4Event;

namespace Shower {
class EventAction : public G4UserEventAction {
public:
#ifdef SHOWER_BENCHMARK
    explicit EventAction(std::shared_ptr<Recorder> recorder, std::shared_ptr<Benchmark> benchmark);
#else
    explicit EventAction(std::shared_ptr<Recorder> recorder);
#endif
    ~EventAction() override;

    void BeginOfEventAction(const G4Event*) override;
    void EndOfEventAction(const G4Event*) override;

private:
#ifdef SHOWER_BENCHMARK
    std::unique_ptr<Benchmark::Measurement> m_measurement;
    std::shared_ptr<Benchmark> m_benchmark;
#endif
    std::shared_ptr<Recorder> m_recorder;
    std::chrono::steady_clock::time_point m_start;
    static size_t c_n;
};
}

#endif // EVENTACTION_H
