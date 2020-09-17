#ifndef CANCELCRITERION_H
#define CANCELCRITERION_H

#include "global.h"

#include <atomic>

namespace Shower {

class CancelCriterion {
public:
    virtual ~CancelCriterion();

    [[nodiscard]] auto met() const -> bool;
    [[nodiscard]] auto ultimate() const -> bool;

protected:
    void set_met();
    void set_ultimate();

private:
    std::atomic<bool> m_met { false };
    std::atomic<bool> m_ultimate { false };
};

class NeverCancel : public CancelCriterion {
public:
    ~NeverCancel() override;
};

class TimedCancel : public CancelCriterion {
public:
    explicit TimedCancel(const std::chrono::seconds min, const std::chrono::seconds max);
    explicit TimedCancel(const std::chrono::seconds middle, const double percent = 0);

    ~TimedCancel() override;
};
}
#endif // CANCELCRITERION_H
