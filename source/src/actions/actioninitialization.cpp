#include "actions/actioninitialization.h"

#include "actions/eventaction.h"
#include "actions/primarygeneratoraction.h"
#include "actions/steppingaction.h"

namespace Shower {
#ifdef SHOWER_BENCHMARK
ActionInitialization::ActionInitialization(const std::shared_ptr<Recorder>& recorder, const std::shared_ptr<Configuration>& configuration, const std::shared_ptr<Benchmark> &benchmark)
    : m_recorder { recorder }
    , m_configuration { configuration }
    , m_benchmark { benchmark }
{
}
#else
ActionInitialization::ActionInitialization(const std::shared_ptr<Recorder>& recorder, const std::shared_ptr<Configuration>& configuration)
    : m_recorder { recorder }
    , m_configuration { configuration }
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
    SetUserAction(new EventAction {m_benchmark});
#else
    SetUserAction(new EventAction {});
#endif

    SetUserAction(new SteppingAction { m_recorder, m_configuration });
}
}
