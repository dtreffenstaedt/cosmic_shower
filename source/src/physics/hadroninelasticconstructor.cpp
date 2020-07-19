#include "physics/hadroninelasticconstructor.h"

#include <iomanip>

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "globals.hh"

#include "G4BaryonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

#include "G4ComponentGGHadronNucleusXsc.hh"
#include "G4CrossSectionDataSetRegistry.hh"

#include "G4HadronCaptureProcess.hh"
#include "G4NeutronCaptureXS.hh"
#include "G4NeutronInelasticXS.hh"
#include "G4NeutronRadCapture.hh"

#include "G4BGGNucleonInelasticXS.hh"
#include "G4BGGPionInelasticXS.hh"
#include "G4ComponentAntiNuclNuclearXS.hh"
#include "G4CrossSectionInelastic.hh"
#include "G4CrossSectionPairGG.hh"
#include "G4ProcessManager.hh"

#include "G4PhysListUtil.hh"

#include "G4HadronicParameters.hh"

// factory
#include "G4PhysicsConstructorFactory.hh"
//
#include "G4ProcessManager.hh"

namespace Shower {
HadronInelasticConstructor::HadronInelasticConstructor(G4int)
    : G4VPhysicsConstructor("hInelastic FTFQGSP_BERT")
{
}

HadronInelasticConstructor::HadronInelasticConstructor(const G4String& name, G4bool /* quasiElastic */)
    : G4VPhysicsConstructor(name)
{
}

void HadronInelasticConstructor::CreateModels()
{
    auto min_transition_ftf = G4HadronicParameters::Instance()->GetMinEnergyTransitionFTF_Cascade();
    auto max_transition_ftf = G4HadronicParameters::Instance()->GetMaxEnergyTransitionFTF_Cascade();

    m_models.string = new G4FTFModel;
    m_models.string->SetMinEnergy(0.0);
    m_models.string->SetMaxEnergy(Config::max_energy);

    m_models.string_decay = new G4ExcitedStringDecay(m_models.QGSM_fragmentation = new G4QGSMFragmentation);
    m_models.string->SetFragmentationModel(m_models.string_decay);
    m_models.pre_equilib = new G4PreCompoundModel(m_models.handler = new G4ExcitationHandler);
    m_models.cascade = new G4GeneratorPrecompoundInterface(m_models.pre_equilib);
    m_models.cascade->SetMinEnergy(0.0);
    m_models.cascade->SetMaxEnergy(Config::max_energy);

    // FTF for neutrons, protons, pions, and kaons
    m_models.model1 = new G4TheoFSGenerator("FTFP");
    m_models.model1->SetMinEnergy(min_transition_ftf);
    m_models.model1->SetMaxEnergy(Config::max_energy);
    m_models.model1->SetTransport(m_models.cascade);
    m_models.model1->SetHighEnergyGenerator(m_models.string);

    m_models.bertini1 = new G4CascadeInterface {};
    m_models.bertini1->SetMinEnergy(0.0);
    m_models.bertini1->SetMaxEnergy(max_transition_ftf);

    // FTF for hyperons
    m_models.model2 = new G4TheoFSGenerator("FTFP");
    m_models.model2->SetMinEnergy(min_transition_ftf);
    m_models.model2->SetMaxEnergy(Config::max_energy);
    m_models.model2->SetTransport(m_models.cascade);
    m_models.model2->SetHighEnergyGenerator(m_models.string);

    // FTF for Antibaryons
    m_models.model3 = new G4TheoFSGenerator("FTFP");
    m_models.model3->SetMinEnergy(0.0);
    m_models.model3->SetMaxEnergy(Config::max_energy);
    m_models.model3->SetTransport(m_models.cascade);
    m_models.model3->SetHighEnergyGenerator(m_models.string);

    // Neutron Capture
    m_models.neutron_capture = new G4NeutronRadCapture;
    m_models.neutron_capture->SetMinEnergy(0.0);
    m_models.neutron_capture->SetMaxEnergy(Config::max_energy);

    m_crossections.pi = new G4BGGPionInelasticXS(G4PionMinus::Definition());
    m_crossections.pi->SetMinKinEnergy(0.0);
    m_crossections.pi->SetMaxKinEnergy(Config::max_energy);

    auto* anti_nucl = new G4ComponentAntiNuclNuclearXS {};
    anti_nucl->SetMinKinEnergy(0.0);
    anti_nucl->SetMaxKinEnergy(Config::max_energy);
    m_crossections.anti_nucleon = new G4CrossSectionInelastic(anti_nucl);
    m_crossections.anti_nucleon->SetMinKinEnergy(0.0);
    m_crossections.anti_nucleon->SetMaxKinEnergy(Config::max_energy);

    auto* gg_hadron_nucl = new G4ComponentGGHadronNucleusXsc {};
    gg_hadron_nucl->SetMinKinEnergy(0.0);
    gg_hadron_nucl->SetMaxKinEnergy(Config::max_energy);

    m_crossections.kaon_and_hyperon = new G4CrossSectionInelastic(gg_hadron_nucl);
    m_crossections.kaon_and_hyperon->SetMinKinEnergy(0.0);
    m_crossections.kaon_and_hyperon->SetMaxKinEnergy(Config::max_energy);

    m_crossections.neutron_inelastic = G4CrossSectionDataSetRegistry::Instance()->GetCrossSectionDataSet(G4NeutronInelasticXS::Default_Name());
    m_crossections.neutron_inelastic->SetMinKinEnergy(0.0);
    m_crossections.neutron_inelastic->SetMaxKinEnergy(Config::max_energy);

    m_crossections.neutron_capture = G4CrossSectionDataSetRegistry::Instance()->GetCrossSectionDataSet(G4NeutronCaptureXS::Default_Name());
    m_crossections.neutron_capture->SetMinKinEnergy(0.0);
    m_crossections.neutron_capture->SetMaxKinEnergy(Config::max_energy);

    m_crossections.neutron_bgc = new G4BGGNucleonInelasticXS { G4Neutron::Neutron() };
    m_crossections.neutron_bgc->SetMinKinEnergy(0.0);
    m_crossections.neutron_bgc->SetMaxKinEnergy(Config::max_energy);

    m_crossections.proton_bgc = new G4BGGNucleonInelasticXS { G4Proton::Proton() };
    m_crossections.proton_bgc->SetMinKinEnergy(0.0);
    m_crossections.proton_bgc->SetMaxKinEnergy(Config::max_energy);
}

HadronInelasticConstructor::~HadronInelasticConstructor()
{
    delete m_models.string_decay;
    delete m_models.string;
    delete m_models.pre_equilib;
    delete m_models.cascade;
    delete m_models.QGSM_fragmentation;
}

void HadronInelasticConstructor::ConstructParticle()
{
    G4MesonConstructor pMesonConstructor;
    pMesonConstructor.ConstructParticle();

    G4BaryonConstructor pBaryonConstructor;
    pBaryonConstructor.ConstructParticle();

    G4ShortLivedConstructor pShortLivedConstructor;
    pShortLivedConstructor.ConstructParticle();
}

void HadronInelasticConstructor::ConstructProcess()
{
    CreateModels();

    G4ProcessManager* aProcMan{nullptr};

    m_processes.neutron_inelastic = new G4NeutronInelasticProcess();
    m_processes.neutron_inelastic->RegisterMe(m_models.model1);
    m_processes.neutron_inelastic->RegisterMe(m_models.bertini1);
    m_processes.neutron_inelastic->AddDataSet(m_crossections.neutron_bgc);
    m_processes.neutron_inelastic->AddDataSet(m_crossections.neutron_inelastic);
    aProcMan = G4Neutron::Neutron()->GetProcessManager();
    aProcMan->AddDiscreteProcess(m_processes.neutron_inelastic);
    m_processes.neutron_capture_process = new G4HadronCaptureProcess();
    m_processes.neutron_capture_process->RegisterMe(m_models.neutron_capture);
    aProcMan->AddDiscreteProcess(m_processes.neutron_capture_process);

    m_processes.proton_inelastic = new G4ProtonInelasticProcess();
    m_processes.proton_inelastic->RegisterMe(m_models.model1);
    m_processes.proton_inelastic->RegisterMe(m_models.bertini1);
    m_processes.proton_inelastic->AddDataSet(m_crossections.proton_bgc);
    aProcMan = G4Proton::Proton()->GetProcessManager();
    aProcMan->AddDiscreteProcess(m_processes.proton_inelastic);

    m_processes.pion_minus_inelastic = new G4PionMinusInelasticProcess();
    m_processes.pion_minus_inelastic->RegisterMe(m_models.model1);
    m_processes.pion_minus_inelastic->RegisterMe(m_models.bertini1);
    m_processes.pion_minus_inelastic->AddDataSet(m_crossections.pi);
    aProcMan = G4PionMinus::PionMinus()->GetProcessManager();
    aProcMan->AddDiscreteProcess(m_processes.pion_minus_inelastic);

    m_processes.pion_plus_inelastic = new G4PionPlusInelasticProcess();
    m_processes.pion_plus_inelastic->RegisterMe(m_models.model1);
    m_processes.pion_plus_inelastic->RegisterMe(m_models.bertini1);
    m_processes.pion_plus_inelastic->AddDataSet(m_crossections.pi);
    aProcMan = G4PionPlus::PionPlus()->GetProcessManager();
    aProcMan->AddDiscreteProcess(m_processes.pion_plus_inelastic);

    m_processes.kaon_minus_inelastic = new G4KaonMinusInelasticProcess();
    m_processes.kaon_minus_inelastic->RegisterMe(m_models.model1);
    m_processes.kaon_minus_inelastic->RegisterMe(m_models.bertini1);
    m_processes.kaon_minus_inelastic->AddDataSet(m_crossections.kaon_and_hyperon);
    aProcMan = G4KaonMinus::KaonMinus()->GetProcessManager();
    aProcMan->AddDiscreteProcess(m_processes.kaon_minus_inelastic);

    m_processes.kaon_plus_inelastic = new G4KaonPlusInelasticProcess();
    m_processes.kaon_plus_inelastic->RegisterMe(m_models.model1);
    m_processes.kaon_plus_inelastic->RegisterMe(m_models.bertini1);
    m_processes.kaon_plus_inelastic->AddDataSet(m_crossections.kaon_and_hyperon);
    aProcMan = G4KaonPlus::KaonPlus()->GetProcessManager();
    aProcMan->AddDiscreteProcess(m_processes.kaon_plus_inelastic);

    m_processes.kaon_zero_l_inelastic = new G4KaonZeroLInelasticProcess();
    m_processes.kaon_zero_l_inelastic->RegisterMe(m_models.model1);
    m_processes.kaon_zero_l_inelastic->RegisterMe(m_models.bertini1);
    m_processes.kaon_zero_l_inelastic->AddDataSet(m_crossections.kaon_and_hyperon);
    aProcMan = G4KaonZeroLong::KaonZeroLong()->GetProcessManager();
    aProcMan->AddDiscreteProcess(m_processes.kaon_zero_l_inelastic);

    m_processes.kaon_zero_s_inelastic = new G4KaonZeroSInelasticProcess();
    m_processes.kaon_zero_s_inelastic->RegisterMe(m_models.model1);
    m_processes.kaon_zero_s_inelastic->RegisterMe(m_models.bertini1);
    m_processes.kaon_zero_s_inelastic->AddDataSet(m_crossections.kaon_and_hyperon);
    aProcMan = G4KaonZeroShort::KaonZeroShort()->GetProcessManager();
    aProcMan->AddDiscreteProcess(m_processes.kaon_zero_s_inelastic);

    m_processes.lambda_inelastic = new G4LambdaInelasticProcess();
    m_processes.lambda_inelastic->RegisterMe(m_models.model2);
    m_processes.lambda_inelastic->RegisterMe(m_models.bertini2);
    m_processes.lambda_inelastic->AddDataSet(m_crossections.kaon_and_hyperon);
    aProcMan = G4Lambda::Lambda()->GetProcessManager();
    aProcMan->AddDiscreteProcess(m_processes.lambda_inelastic);

    m_processes.anti_lambda_inelastic = new G4AntiLambdaInelasticProcess();
    m_processes.anti_lambda_inelastic->RegisterMe(m_models.model3);
    m_processes.anti_lambda_inelastic->AddDataSet(m_crossections.kaon_and_hyperon);
    aProcMan = G4AntiLambda::AntiLambda()->GetProcessManager();
    aProcMan->AddDiscreteProcess(m_processes.anti_lambda_inelastic);

    m_processes.sigma_minus_inelastic = new G4SigmaMinusInelasticProcess();
    m_processes.sigma_minus_inelastic->RegisterMe(m_models.model2);
    m_processes.sigma_minus_inelastic->RegisterMe(m_models.bertini2);
    m_processes.sigma_minus_inelastic->AddDataSet(m_crossections.kaon_and_hyperon);
    aProcMan = G4SigmaMinus::SigmaMinus()->GetProcessManager();
    aProcMan->AddDiscreteProcess(m_processes.sigma_minus_inelastic);

    m_processes.anti_sigma_minus_inelastic = new G4AntiSigmaMinusInelasticProcess();
    m_processes.anti_sigma_minus_inelastic->RegisterMe(m_models.model3);
    m_processes.anti_sigma_minus_inelastic->AddDataSet(m_crossections.kaon_and_hyperon);
    aProcMan = G4AntiSigmaMinus::AntiSigmaMinus()->GetProcessManager();
    aProcMan->AddDiscreteProcess(m_processes.anti_sigma_minus_inelastic);

    m_processes.sigma_plus_inelastic = new G4SigmaPlusInelasticProcess();
    m_processes.sigma_plus_inelastic->RegisterMe(m_models.model2);
    m_processes.sigma_plus_inelastic->RegisterMe(m_models.bertini2);
    m_processes.sigma_plus_inelastic->AddDataSet(m_crossections.kaon_and_hyperon);
    aProcMan = G4SigmaPlus::SigmaPlus()->GetProcessManager();
    aProcMan->AddDiscreteProcess(m_processes.sigma_plus_inelastic);

    m_processes.anti_sigma_plus_inelastic = new G4AntiSigmaPlusInelasticProcess();
    m_processes.anti_sigma_plus_inelastic->RegisterMe(m_models.model3);
    m_processes.anti_sigma_plus_inelastic->AddDataSet(m_crossections.kaon_and_hyperon);
    aProcMan = G4AntiSigmaPlus::AntiSigmaPlus()->GetProcessManager();
    aProcMan->AddDiscreteProcess(m_processes.anti_sigma_plus_inelastic);

    m_processes.xi_minus_inelastic = new G4XiMinusInelasticProcess();
    m_processes.xi_minus_inelastic->RegisterMe(m_models.model2);
    m_processes.xi_minus_inelastic->RegisterMe(m_models.bertini2);
    m_processes.xi_minus_inelastic->AddDataSet(m_crossections.kaon_and_hyperon);
    aProcMan = G4XiMinus::XiMinus()->GetProcessManager();
    aProcMan->AddDiscreteProcess(m_processes.xi_minus_inelastic);

    m_processes.anti_xi_minus_inelastic = new G4AntiXiMinusInelasticProcess();
    m_processes.anti_xi_minus_inelastic->RegisterMe(m_models.model3);
    m_processes.anti_xi_minus_inelastic->AddDataSet(m_crossections.kaon_and_hyperon);
    aProcMan = G4AntiXiMinus::AntiXiMinus()->GetProcessManager();
    aProcMan->AddDiscreteProcess(m_processes.anti_xi_minus_inelastic);

    m_processes.xi_zero_inelastic = new G4XiZeroInelasticProcess();
    m_processes.xi_zero_inelastic->RegisterMe(m_models.model2);
    m_processes.xi_zero_inelastic->RegisterMe(m_models.bertini2);
    m_processes.xi_zero_inelastic->AddDataSet(m_crossections.kaon_and_hyperon);
    aProcMan = G4XiZero::XiZero()->GetProcessManager();
    aProcMan->AddDiscreteProcess(m_processes.xi_zero_inelastic);

    m_processes.anti_xi_zero_inelastic = new G4AntiXiZeroInelasticProcess();
    m_processes.anti_xi_zero_inelastic->RegisterMe(m_models.model3);
    m_processes.anti_xi_zero_inelastic->AddDataSet(m_crossections.kaon_and_hyperon);
    aProcMan = G4AntiXiZero::AntiXiZero()->GetProcessManager();
    aProcMan->AddDiscreteProcess(m_processes.anti_xi_zero_inelastic);

    m_processes.omega_minus_inelastic = new G4OmegaMinusInelasticProcess();
    m_processes.omega_minus_inelastic->RegisterMe(m_models.model2);
    m_processes.omega_minus_inelastic->RegisterMe(m_models.bertini2);
    m_processes.omega_minus_inelastic->AddDataSet(m_crossections.kaon_and_hyperon);
    aProcMan = G4OmegaMinus::OmegaMinus()->GetProcessManager();
    aProcMan->AddDiscreteProcess(m_processes.omega_minus_inelastic);

    m_processes.anti_omega_minus_inelastic = new G4AntiOmegaMinusInelasticProcess();
    m_processes.anti_omega_minus_inelastic->RegisterMe(m_models.model3);
    m_processes.anti_omega_minus_inelastic->AddDataSet(m_crossections.kaon_and_hyperon);
    aProcMan = G4AntiOmegaMinus::AntiOmegaMinus()->GetProcessManager();
    aProcMan->AddDiscreteProcess(m_processes.anti_omega_minus_inelastic);

    m_processes.anti_proton_inelastic = new G4AntiProtonInelasticProcess();
    m_processes.anti_proton_inelastic->RegisterMe(m_models.model3);
    m_processes.anti_proton_inelastic->AddDataSet(m_crossections.anti_nucleon);
    aProcMan = G4AntiProton::AntiProton()->GetProcessManager();
    aProcMan->AddDiscreteProcess(m_processes.anti_proton_inelastic);

    m_processes.anti_neutron_inelastic = new G4AntiNeutronInelasticProcess();
    m_processes.anti_neutron_inelastic->RegisterMe(m_models.model3);
    m_processes.anti_neutron_inelastic->AddDataSet(m_crossections.anti_nucleon);
    aProcMan = G4AntiNeutron::AntiNeutron()->GetProcessManager();
    aProcMan->AddDiscreteProcess(m_processes.anti_neutron_inelastic);

    m_processes.anti_deuteron_inelastic = new G4AntiDeuteronInelasticProcess();
    m_processes.anti_deuteron_inelastic->RegisterMe(m_models.model3);
    m_processes.anti_deuteron_inelastic->AddDataSet(m_crossections.anti_nucleon);
    aProcMan = G4AntiDeuteron::AntiDeuteron()->GetProcessManager();
    aProcMan->AddDiscreteProcess(m_processes.anti_deuteron_inelastic);

    m_processes.anti_triton_inelastic = new G4AntiTritonInelasticProcess();
    m_processes.anti_triton_inelastic->RegisterMe(m_models.model3);
    m_processes.anti_triton_inelastic->AddDataSet(m_crossections.anti_nucleon);
    aProcMan = G4AntiTriton::AntiTriton()->GetProcessManager();
    aProcMan->AddDiscreteProcess(m_processes.anti_triton_inelastic);

    m_processes.anti_he3_inelastic = new G4AntiHe3InelasticProcess();
    m_processes.anti_he3_inelastic->RegisterMe(m_models.model3);
    m_processes.anti_he3_inelastic->AddDataSet(m_crossections.anti_nucleon);
    aProcMan = G4AntiHe3::AntiHe3()->GetProcessManager();
    aProcMan->AddDiscreteProcess(m_processes.anti_he3_inelastic);

    m_processes.anti_alpha_inelastic = new G4AntiAlphaInelasticProcess();
    m_processes.anti_alpha_inelastic->RegisterMe(m_models.model3);
    m_processes.anti_alpha_inelastic->AddDataSet(m_crossections.anti_nucleon);
    aProcMan = G4AntiAlpha::AntiAlpha()->GetProcessManager();
    aProcMan->AddDiscreteProcess(m_processes.anti_alpha_inelastic);
}
}
