#ifndef CANCELCRITERION_H
#define CANCELCRITERION_H

#include "global.h"

#include <atomic>

namespace Shower {

class CancelCriterion {
public:
    virtual ~CancelCriterion();

    [[nodiscard]] auto met() const -> bool;

protected:
    void set_met();

private:
    std::atomic<bool> m_met { false };
};

class NeverCancel : public CancelCriterion {
public:
    ~NeverCancel() override;
};

class TimedCancel : public CancelCriterion {
public:
    explicit TimedCancel(const std::chrono::seconds time);

    ~TimedCancel() override;
};
}
#endif // CANCELCRITERION_H
