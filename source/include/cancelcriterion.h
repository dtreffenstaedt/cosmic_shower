#ifndef CANCELCRITERION_H
#define CANCELCRITERION_H

#include "global.h"

#include <atomic>

namespace Shower {

class CancelCriterion
{
public:
    virtual ~CancelCriterion();

    [[nodiscard]] virtual auto met() const -> bool = 0;
};

class NeverCancel : public CancelCriterion
{
public:
    ~NeverCancel() override;

    [[nodiscard]] auto met() const -> bool override;
};

class TimedCancel : public CancelCriterion
{
public:
    explicit TimedCancel(const std::chrono::minutes time);

    ~TimedCancel() override;

    [[nodiscard]] auto met() const -> bool override;

private:
    std::atomic<bool> m_met{false};
};
}
#endif // CANCELCRITERION_H
