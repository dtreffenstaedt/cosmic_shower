#include "cancelcriterion.h"

#include <thread>

namespace Shower {

CancelCriterion::~CancelCriterion() = default;

NeverCancel::~NeverCancel() = default;

auto NeverCancel::met() const -> bool
{
    return false;
}

TimedCancel::TimedCancel(const std::chrono::minutes time)
{
    std::thread([this, &time] {
        std::this_thread::yield();
        std::this_thread::sleep_for(time);
        m_met = true;
    }).detach();
}

TimedCancel::~TimedCancel() = default;

auto TimedCancel::met() const -> bool
{
    return m_met;
}
}
