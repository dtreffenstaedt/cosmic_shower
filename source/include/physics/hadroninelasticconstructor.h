#ifndef HADRONINELASTICCONSTRUCTOR_H
#define HADRONINELASTICCONSTRUCTOR_H

#include "global.h"

#include "G4VPhysicsConstructor.hh"

#include "G4NeutronRadCapture.hh"

#include "G4CascadeInterface.hh"
#include "G4ExcitationHandler.hh"
#include "G4ExcitedStringDecay.hh"
#include "G4FTFModel.hh"
#include "G4GeneratorPrecompoundInterface.hh"
#include "G4PreCompoundModel.hh"
#include "G4QGSMFragmentation.hh"
#include "G4TheoFSGenerator.hh"

#include "G4HadronCaptureProcess.hh"

#include "G4KaonMinusInelasticProcess.hh"
#include "G4KaonPlusInelasticProcess.hh"
#include "G4KaonZeroLInelasticProcess.hh"
#include "G4KaonZeroSInelasticProcess.hh"
#include "G4NeutronInelasticProcess.hh"
#include "G4PionMinusInelasticProcess.hh"
#include "G4PionPlusInelasticProcess.hh"
#include "G4ProtonInelasticProcess.hh"

#include "G4AntiLambdaInelasticProcess.hh"
#include "G4AntiOmegaMinusInelasticProcess.hh"
#include "G4AntiSigmaMinusInelasticProcess.hh"
#include "G4AntiSigmaPlusInelasticProcess.hh"
#include "G4AntiXiMinusInelasticProcess.hh"
#include "G4AntiXiZeroInelasticProcess.hh"
#include "G4LambdaInelasticProcess.hh"
#include "G4OmegaMinusInelasticProcess.hh"
#include "G4SigmaMinusInelasticProcess.hh"
#include "G4SigmaPlusInelasticProcess.hh"
#include "G4XiMinusInelasticProcess.hh"
#include "G4XiZeroInelasticProcess.hh"

#include "G4AntiAlphaInelasticProcess.hh"
#include "G4AntiDeuteronInelasticProcess.hh"
#include "G4AntiHe3InelasticProcess.hh"
#include "G4AntiNeutronInelasticProcess.hh"
#include "G4AntiProtonInelasticProcess.hh"
#include "G4AntiTritonInelasticProcess.hh"
#include "G4BGGNucleonInelasticXS.hh"

namespace Shower {
class HadronInelasticConstructor : public G4VPhysicsConstructor {
public:
    explicit HadronInelasticConstructor(G4int verbose = 1);
    explicit HadronInelasticConstructor(const G4String& name, G4bool quasiElastic = false);
    virtual ~HadronInelasticConstructor();

public:
    virtual void ConstructParticle();
    virtual void ConstructProcess();

private:
    void CreateModels();

    struct
    {
        G4TheoFSGenerator* model1 { nullptr };
        G4TheoFSGenerator* model2 { nullptr };
        G4TheoFSGenerator* model3 { nullptr };

        G4CascadeInterface* bertini1 { nullptr };
        G4CascadeInterface* bertini2 { nullptr };

        G4NeutronRadCapture* neutron_capture { nullptr };

        G4FTFModel* string { nullptr };
        G4ExcitedStringDecay* string_decay { nullptr };

        G4PreCompoundModel* pre_equilib { nullptr };
        G4GeneratorPrecompoundInterface* cascade { nullptr };
        G4QGSMFragmentation* QGSM_fragmentation { nullptr };
        G4ExcitationHandler* handler { nullptr };
    } m_models {};

    struct
    {
        G4HadronCaptureProcess* neutron_capture_process { nullptr };

        G4NeutronInelasticProcess* neutron_inelastic { nullptr };
        G4ProtonInelasticProcess* proton_inelastic { nullptr };
        G4PionMinusInelasticProcess* pion_minus_inelastic { nullptr };
        G4PionPlusInelasticProcess* pion_plus_inelastic { nullptr };
        G4KaonMinusInelasticProcess* kaon_minus_inelastic { nullptr };
        G4KaonPlusInelasticProcess* kaon_plus_inelastic { nullptr };
        G4KaonZeroLInelasticProcess* kaon_zero_l_inelastic { nullptr };
        G4KaonZeroSInelasticProcess* kaon_zero_s_inelastic { nullptr };

        G4LambdaInelasticProcess* lambda_inelastic { nullptr };
        G4AntiLambdaInelasticProcess* anti_lambda_inelastic { nullptr };
        G4SigmaMinusInelasticProcess* sigma_minus_inelastic { nullptr };
        G4AntiSigmaMinusInelasticProcess* anti_sigma_minus_inelastic { nullptr };
        G4SigmaPlusInelasticProcess* sigma_plus_inelastic { nullptr };
        G4AntiSigmaPlusInelasticProcess* anti_sigma_plus_inelastic { nullptr };
        G4XiZeroInelasticProcess* xi_zero_inelastic { nullptr };
        G4AntiXiZeroInelasticProcess* anti_xi_zero_inelastic { nullptr };
        G4XiMinusInelasticProcess* xi_minus_inelastic { nullptr };
        G4AntiXiMinusInelasticProcess* anti_xi_minus_inelastic { nullptr };
        G4OmegaMinusInelasticProcess* omega_minus_inelastic { nullptr };
        G4AntiOmegaMinusInelasticProcess* anti_omega_minus_inelastic { nullptr };

        G4AntiProtonInelasticProcess* anti_proton_inelastic { nullptr };
        G4AntiNeutronInelasticProcess* anti_neutron_inelastic { nullptr };
        G4AntiDeuteronInelasticProcess* anti_deuteron_inelastic { nullptr };
        G4AntiTritonInelasticProcess* anti_triton_inelastic { nullptr };
        G4AntiHe3InelasticProcess* anti_he3_inelastic { nullptr };
        G4AntiAlphaInelasticProcess* anti_alpha_inelastic { nullptr };
    } m_processes {};

    struct
    {
        G4VCrossSectionDataSet* pi { nullptr };
        G4VCrossSectionDataSet* kaon_and_hyperon { nullptr };
        G4VCrossSectionDataSet* anti_nucleon { nullptr };
        G4VCrossSectionDataSet* neutron_inelastic { nullptr };
        G4VCrossSectionDataSet* neutron_capture { nullptr };
        G4BGGNucleonInelasticXS* neutron_bgc { nullptr };
        G4BGGNucleonInelasticXS* proton_bgc { nullptr };
    } m_crossections {};
};
}

#endif // HADRONINELASTICCONSTRUCTOR_H
