#ifndef RECORDERMANAGER_H
#define RECORDERMANAGER_H

#include "global.h"

#include <atomic>
#include <queue>

#include <G4ThreeVector.hh>

namespace Shower
{

class RecorderManager
{
public:
    RecorderManager();
    virtual ~RecorderManager();

    bool stored_primary() const;

    void store_primary(const G4ThreeVector& vector, const G4double& time);
    void store_hit(const G4ThreeVector& vector, const G4double& time);

    static RecorderManager* singleton();
private:
    struct Hit
    {
        G4ThreeVector pos;
        G4double time;
    };
    void save();

    static RecorderManager* c_singleton;

    std::atomic<bool> m_stored_primary;

    Hit m_primary;

    std::queue<Hit> m_hits;

};
}

#endif // RECORDERMANAGER_H
