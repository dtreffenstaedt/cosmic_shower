#include "cancelcriterion.h"

#include <iostream>
#include <thread>

namespace Shower {

CancelCriterion::~CancelCriterion() = default;

auto CancelCriterion::met() const -> bool
{
    return m_met;
}

auto CancelCriterion::ultimate() const -> bool
{
    return m_ultimate;
}

void CancelCriterion::set_met()
{
    m_met = true;
}

void CancelCriterion::set_ultimate()
{
    set_met();
    m_ultimate = true;
}

NeverCancel::~NeverCancel() = default;

TimedCancel::TimedCancel(const std::chrono::seconds min, const std::chrono::seconds max)
{
    std::thread([this, min, max] {
        auto start = std::chrono::steady_clock::now();
        while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start).count() < min.count()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(4000));
        }
        set_met();
        while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start).count() < max.count()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(4000));
        }
        set_ultimate();
    }).detach();
}

TimedCancel::TimedCancel(const std::chrono::seconds middle, const double percent)
    : TimedCancel{std::chrono::seconds{static_cast<long>(static_cast<double>(middle.count()) * (1 - percent))}, std::chrono::seconds{static_cast<long>(static_cast<double>(middle.count()) * (1 + percent))}}
{
}

TimedCancel::~TimedCancel() = default;

}
