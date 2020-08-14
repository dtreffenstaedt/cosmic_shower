#include "cancelcriterion.h"

#include <iostream>
#include <thread>

namespace Shower {

CancelCriterion::~CancelCriterion() = default;

auto CancelCriterion::met() const -> bool
{
    return m_met;
}

void CancelCriterion::set_met()
{
    m_met = true;
}

NeverCancel::~NeverCancel() = default;

TimedCancel::TimedCancel(const std::chrono::seconds time)
{
    std::thread([this, time] {
        auto start = std::chrono::steady_clock::now();
        while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start).count() < time.count()) {
            std::cout << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start).count() << '(' << time.count() << ")\n"
                      << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(4000));
        }
        set_met();
        std::cout << "finishing!\n"
                  << std::flush;
    }).detach();
}

TimedCancel::~TimedCancel() = default;

}
