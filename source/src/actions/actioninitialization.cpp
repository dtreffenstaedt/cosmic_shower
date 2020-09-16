#include "actions/actioninitialization.h"

#include <utility>

#include "actions/eventaction.h"
#include "actions/primarygeneratoraction.h"
#include "actions/steppingaction.h"
#include "actions/stackingaction.h"

#include "cancelcriterion.h"
#include "configuration.h"
#include "recorder.h"
#ifdef SHOWER_BENCHMARK
#include "benchmark.h"
#endif

namespace Shower {
#ifdef SHOWER_BENCHMARK
ActionInitialization::ActionInitialization(std::shared_ptr<Recorder> recorder, std::shared_ptr<Configuration> configuration, std::shared_ptr<Benchmark> benchmark)
    : m_recorder { std::move(recorder) }
    , m_configuration { std::move(configuration) }
    , m_benchmark { std::move(benchmark) }
{
}
#else
ActionInitialization::ActionInitialization(std::shared_ptr<Recorder> recorder, std::shared_ptr<Configuration> configuration)
    : m_recorder { std::move(recorder) }
    , m_configuration { std::move(configuration) }
{
}
#endif

void ActionInitialization::BuildForMaster() const
{
}

void ActionInitialization::Build() const
{
    SetUserAction(new PrimaryGeneratorAction { m_configuration });

#ifdef SHOWER_BENCHMARK
    SetUserAction(new EventAction { m_recorder, m_benchmark });
#else
    SetUserAction(new EventAction { m_recorder });
#endif

    SetUserAction(new SteppingAction { m_recorder, m_configuration });

    SetUserAction(new StackingAction {});
}
}
