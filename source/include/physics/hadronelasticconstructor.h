#ifndef HADRONELASTICCONSTRUCTOR_H
#define HADRONELASTICCONSTRUCTOR_H

#include "global.h"

#include "G4HadronElastic.hh"
#include "G4HadronicProcess.hh"
#include "G4VPhysicsConstructor.hh"

class G4VCrossSectionDataSet;
class G4ParticleDefinition;

namespace Shower {

class HadronElasticConstructor : public G4VPhysicsConstructor {
public:
    explicit HadronElasticConstructor(G4int ver = 0,
        const G4String& nam = "hElasticWEL_CHIPS_XS");

    virtual ~HadronElasticConstructor();

    void ConstructParticle() override;

    void ConstructProcess() override;

    static G4HadronicProcess* GetElasticProcess(const G4ParticleDefinition* part);

    static G4HadronElastic* GetElasticModel(const G4ParticleDefinition* part);

    static G4HadronicProcess* GetNeutronProcess();

    static G4HadronElastic* GetNeutronModel();

    static void AddXSection(const G4ParticleDefinition*, G4VCrossSectionDataSet*);

private:
    // copy constructor and hide assignment operator
    HadronElasticConstructor(HadronElasticConstructor&) = delete;
    HadronElasticConstructor& operator=(const HadronElasticConstructor& right) = delete;

protected:
    G4int verbose;
};
}

#endif // HADRONELASTICCONSTRUCTOR_H
