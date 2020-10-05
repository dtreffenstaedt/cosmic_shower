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
    , m_cancel_criterion {std::move(cancel_criterion)}
{
}
#else
ActionInitialization::ActionInitialization(std::shared_ptr<Recorder> recorder, std::shared_ptr<Configuration> configuration, std::shared_ptr<CancelCriterion> cancel_criterion)
    : m_recorder { std::move(recorder) }
    , m_configuration { std::move(configuration) }
    , m_cancel_criterion {std::move(cancel_criterion)}
{
}
#endif

void ActionInitialization::BuildForMaster() const
{
}

void ActionInitialization::Build() const
{
    SetUserAction(new PrimaryGeneratorAction {m_recorder, m_configuration });

#ifdef SHOWER_BENCHMARK
    SetUserAction(new EventAction { m_recorder, m_benchmark });
#else
    SetUserAction(new EventAction { m_recorder });
#endif

    SetUserAction(new SteppingAction { m_recorder, m_configuration });

    SetUserAction(new StackingAction {m_recorder, m_cancel_criterion});
}
}
