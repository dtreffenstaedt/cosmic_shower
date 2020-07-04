#ifndef EVENTACTION_H
#define EVENTACTION_H


#include "global.h"

#ifdef SHOWER_BENCHMARK
#include "timemanager.h"
#endif


#include <cstddef>
#include <G4UserEventAction.hh>


class G4Event;

START_NAMESPACE
{
class EventAction : public G4UserEventAction
{
public:
    EventAction();
    virtual ~EventAction();

    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);

private:
#ifdef SHOWER_BENCHMARK
    std::unique_ptr<TimeManager::Measurement> m_measurement;
#endif
    static size_t c_n;
};
}

#endif // EVENTACTION_H
