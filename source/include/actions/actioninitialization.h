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
    explicit ActionInitialization(std::shared_ptr<Recorder> recorder, std::shared_ptr<Configuration> configuration, std::shared_ptr<Benchmark> benchmark, std::shared_ptr<CancelCriterion> cancel_criterion);
#else
    explicit ActionInitialization(std::shared_ptr<Recorder> recorder, std::shared_ptr<Configuration> configuration, std::shared_ptr<CancelCriterion> cancel_criterion);
#endif

    void BuildForMaster() const override;
    void Build() const override;

private:
    std::shared_ptr<Recorder> m_recorder{nullptr};
    std::shared_ptr<Configuration> m_configuration{nullptr};
#ifdef SHOWER_BENCHMARK
    std::shared_ptr<Benchmark> m_benchmark{nullptr};
#endif
    std::shared_ptr<CancelCriterion> m_cancel_criterion{nullptr};
};
}

#endif // ACTIONINITIALIZATION_H
