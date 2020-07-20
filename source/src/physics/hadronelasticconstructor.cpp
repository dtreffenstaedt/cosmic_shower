#include "physics/hadronelasticconstructor.h"

#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4SystemOfUnits.hh"

#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4MesonConstructor.hh"

#include "G4AntiNuclElastic.hh"
#include "G4ChipsElasticModel.hh"
#include "G4ElasticHadrNucleusHE.hh"
#include "G4HadronElastic.hh"
#include "G4HadronElasticProcess.hh"

#include "G4ComponentGGHadronNucleusXsc.hh"
#include "G4ComponentGGNuclNuclXsc.hh"

#include "G4BGGNucleonElasticXS.hh"
#include "G4BGGPionElasticXS.hh"

#include "G4CrossSectionDataSetRegistry.hh"

#include "G4ChipsProtonElasticXS.hh"

#include "G4NeutronElasticXS.hh"

#include "G4CrossSectionElastic.hh"

namespace Shower {

HadronElasticConstructor::HadronElasticConstructor(G4int ver, const G4String& nam)
    : G4VPhysicsConstructor(nam)
    , verbose(ver)
{
}

HadronElasticConstructor::~HadronElasticConstructor()
    = default;

void HadronElasticConstructor::ConstructParticle()
{
    // G4cout << "HadronElasticConstructor::ConstructParticle" << G4endl;
    G4MesonConstructor pMesonConstructor;
    G4MesonConstructor::ConstructParticle();

    G4BaryonConstructor pBaryonConstructor;
    G4BaryonConstructor::ConstructParticle();

    G4IonConstructor pConstructor;
    G4IonConstructor::ConstructParticle();
}

void HadronElasticConstructor::ConstructProcess()
{
    constexpr G4double elim_anti_nuc = 100 * MeV;
    constexpr G4double delta = 0.1 * MeV;

    auto* anuc = new G4AntiNuclElastic();
    anuc->SetMinEnergy(elim_anti_nuc);
    anuc->SetMaxEnergy(Config::max_energy);
    auto* anucxs = new G4CrossSectionElastic(anuc->GetComponentCrossSection());
    //    anucxs->SetMinKinEnergy(elim_anti_nuc);
    anucxs->SetMaxKinEnergy(Config::max_energy);

    auto* lhep0 = new G4HadronElastic();
    lhep0->SetMaxEnergy(elim_anti_nuc + delta);
    //    lhep0->SetMaxEnergy(Config::max_energy);

    auto* he = new G4ElasticHadrNucleusHE();
    //    he->SetMinEnergy(100 * TeV);
    he->SetMaxEnergy(Config::max_energy);

    auto* gg_hadron_nucl = new G4ComponentGGHadronNucleusXsc {};
    //    gg_hadron_nucl->SetMinKinEnergy(0.0);
    gg_hadron_nucl->SetMaxKinEnergy(Config::max_energy);

    G4VCrossSectionDataSet* theComponentGGHadronNucleusData = new G4CrossSectionElastic(gg_hadron_nucl);
    //    theComponentGGHadronNucleusData->SetMinKinEnergy(0.0);
    theComponentGGHadronNucleusData->SetMaxKinEnergy(Config::max_energy);

    auto* gg_nucl_nucl = new G4ComponentGGNuclNuclXsc {};
    //    gg_nucl_nucl->SetMinKinEnergy(100 * TeV);
    gg_nucl_nucl->SetMaxKinEnergy(Config::max_energy);

    G4VCrossSectionDataSet* theComponentGGNuclNuclData = new G4CrossSectionElastic(gg_nucl_nucl);
    //    theComponentGGNuclNuclData->SetMinKinEnergy(100 * TeV);
    theComponentGGNuclNuclData->SetMaxKinEnergy(Config::max_energy);

    G4HadronElasticProcess* hel { nullptr };

    auto* myParticleIterator = GetParticleIterator();
    myParticleIterator->reset();
    while ((*myParticleIterator)()) {
        G4ParticleDefinition* particle = myParticleIterator->value();
        G4ProcessManager* pmanager = particle->GetProcessManager();
        G4String pname = particle->GetParticleName();
        if (pname == "anti_lambda" || pname == "anti_omega-" || pname == "anti_sigma-" || pname == "anti_sigma0" || pname == "anti_sigma+" || pname == "anti_xi-" || pname == "anti_xi0" || pname == "lambda" || pname == "omega-" || pname == "sigma-" || pname == "sigma0" || pname == "sigma+" || pname == "xi-" || pname == "xi0") {

            hel = new G4HadronElasticProcess();
            hel->RegisterMe(lhep0);
            hel->AddDataSet(theComponentGGHadronNucleusData);
            pmanager->AddDiscreteProcess(hel);

        } else if (pname == "alpha" || pname == "deuteron" || pname == "triton" || pname == "He3") {
            hel = new G4HadronElasticProcess();
            hel->AddDataSet(theComponentGGNuclNuclData);
            hel->RegisterMe(lhep0);
            pmanager->AddDiscreteProcess(hel);

        } else if (pname == "proton") {

            hel = new G4HadronElasticProcess {};
            auto* nuc_elastic = new G4BGGNucleonElasticXS { particle };
            //            nuc_elastic->SetMinKinEnergy(100 * TeV);
            nuc_elastic->SetMaxKinEnergy(Config::max_energy);
            auto* chips = new G4ChipsElasticModel();
            //            chips->SetMinEnergy(100 * TeV);
            chips->SetMaxEnergy(Config::max_energy);
            hel->AddDataSet(nuc_elastic);
            hel->RegisterMe(chips);
            pmanager->AddDiscreteProcess(hel);

        } else if (pname == "neutron") {

            hel = new G4HadronElasticProcess {};
            auto* nuc_elastic = new G4NeutronElasticXS {};
            //            nuc_elastic->SetMinKinEnergy(100 * TeV);
            nuc_elastic->SetMaxKinEnergy(Config::max_energy);
            auto* chips = new G4ChipsElasticModel();
            //            chips->SetMinEnergy(100 * TeV);
            chips->SetMaxEnergy(Config::max_energy);
            hel->AddDataSet(nuc_elastic);
            hel->RegisterMe(chips);
            hel->AddDataSet(nuc_elastic);
            hel->RegisterMe(chips);
            pmanager->AddDiscreteProcess(hel);

        } else if (pname == "pi+" || pname == "pi-") {

            hel = new G4HadronElasticProcess {};
            auto* nuc_elastic = new G4BGGPionElasticXS { particle };
            //            nuc_elastic->SetMinKinEnergy(100 * TeV);
            nuc_elastic->SetMaxKinEnergy(Config::max_energy);
            hel->AddDataSet(nuc_elastic);
            hel->RegisterMe(he);
            pmanager->AddDiscreteProcess(hel);

        } else if (pname == "kaon-" || pname == "kaon+" || pname == "kaon0S" || pname == "kaon0L") {

            hel = new G4HadronElasticProcess {};
            hel->AddDataSet(theComponentGGHadronNucleusData);
            hel->RegisterMe(lhep0);
            pmanager->AddDiscreteProcess(hel);

        } else if (
            pname == "anti_proton" || pname == "anti_neutron" || pname == "anti_alpha" || pname == "anti_deuteron" || pname == "anti_triton" || pname == "anti_He3") {

            hel = new G4HadronElasticProcess {};
            hel->AddDataSet(anucxs);
            hel->RegisterMe(anuc);
            pmanager->AddDiscreteProcess(hel);
        }
    }
}

auto HadronElasticConstructor::GetElasticProcess(const G4ParticleDefinition* part) -> G4HadronicProcess*
{
    G4HadronicProcess* hp { nullptr };
    G4ProcessVector* pv = part->GetProcessManager()->GetPostStepProcessVector();
    size_t n = pv->size();
    for (size_t i = 0; i < n; ++i) {
        if ((*pv)[i]->GetProcessSubType() == fHadronElastic) {
            hp = static_cast<G4HadronicProcess*>((*pv)[i]);
            break;
        }
    }
    return hp;
}

auto HadronElasticConstructor::GetElasticModel(const G4ParticleDefinition* part) -> G4HadronElastic*
{
    G4HadronElastic* mod { nullptr };
    G4HadronicProcess* hel = GetElasticProcess(part);
    if (hel != nullptr) {
        std::vector<G4HadronicInteraction*>& hi = hel->GetHadronicInteractionList();
        if (!hi.empty()) {
            mod = static_cast<G4HadronElastic*>(hi[0]);
        }
    }
    return mod;
}

auto HadronElasticConstructor::GetNeutronProcess() -> G4HadronicProcess*
{
    return GetElasticProcess(G4Neutron::Neutron());
}

auto HadronElasticConstructor::GetNeutronModel() -> G4HadronElastic*
{
    return GetElasticModel(G4Neutron::Neutron());
}

void HadronElasticConstructor::AddXSection(const G4ParticleDefinition* part,
    G4VCrossSectionDataSet* cross)
{
    G4HadronicProcess* hel = GetElasticProcess(part);
    if (hel != nullptr) {
        hel->AddDataSet(cross);
    }
}
}
