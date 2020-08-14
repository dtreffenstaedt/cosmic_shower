#ifndef ACTIONINITIALIZATION_H
#define ACTIONINITIALIZATION_H

#include <G4VUserActionInitialization.hh>
#include <globals.hh>

#include "global.h"

namespace Shower {

class Recorder;
class Configuration;
class CancelCriterion;
#ifdef SHOWER_BENCHMARK
class Benchmark;
#endif

class ActionInitialization : public G4VUserActionInitialization {
public:
#ifdef SHOWER_BENCHMARK
    explicit ActionInitialization(std::shared_ptr<Recorder> recorder, std::shared_ptr<Configuration> configuration, std::shared_ptr<Benchmark> benchmark);
#else
    explicit ActionInitialization(std::shared_ptr<Recorder> recorder, std::shared_ptr<Configuration> configuration);
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
