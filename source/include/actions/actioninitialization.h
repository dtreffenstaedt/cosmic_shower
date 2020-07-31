#ifndef ACTIONINITIALIZATION_H
#define ACTIONINITIALIZATION_H

#include <G4VUserActionInitialization.hh>
#include <globals.hh>

#include "global.h"
#ifdef SHOWER_BENCHMARK
#include "benchmark.h"
#endif

namespace Shower {
class ActionInitialization : public G4VUserActionInitialization {
public:
#ifdef SHOWER_BENCHMARK
    explicit ActionInitialization(const std::shared_ptr<Recorder>& recorder, const std::shared_ptr<Configuration>& configuration, const std::shared_ptr<Benchmark>& benchmark);
#else
    explicit ActionInitialization(const std::shared_ptr<Recorder>& recorder, const std::shared_ptr<Configuration>& configuration);
#endif

    void BuildForMaster() const override;
    void Build() const override;

private:
    std::shared_ptr<Recorder> m_recorder;
    std::shared_ptr<Configuration> m_configuration;
#ifdef SHOWER_BENCHMARK
    std::shared_ptr<Benchmark> m_benchmark;
#endif
};
}

#endif // ACTIONINITIALIZATION_H
