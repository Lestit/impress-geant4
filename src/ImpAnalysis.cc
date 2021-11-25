#include <ctime>
#include <chrono>
#include <iomanip>
#include <memory>
#include <map>
#include <numeric>
#include <sstream>
#include <string>

#include "g4analysis.hh"
#include "G4AccumulableManager.hh"
#include "G4AutoLock.hh"
#include "G4Event.hh"
#include "G4Run.hh"
#include "G4SystemOfUnits.hh"
#include "G4THitsCollection.hh"
#include "G4VHitsCollection.hh"

#include "ImpAnalysis.hh"
#include "ImpScintCrystalHit.hh"
#include "ImpVHit.hh"

namespace {
    ImpAnalysis *anInst = nullptr;
    G4Mutex instMux = G4MUTEX_INITIALIZER;
    G4Mutex dataMux = G4MUTEX_INITIALIZER;

    static const G4String MANAGER_TYPE = "csv";
    static const G4String HISTO_DIRECTORY = "impress-histo";
    static const G4String TUPLE_DIRECTORY = "impress-tuple";
    static const G4String OUT_SPEC_FN_PFX = "spec-out-";
    static const G4String IN_SPEC_FN_PFX  = "spec-in-";
    static const G4String SI_OUT_FN_PFX = "si-out-";

    static const G4String ENERGY_NTUPLE_NAME = "depetup";
}

ImpAnalysis::ImpAnalysis() :
    totalEvents(0),
    savePositions(false)
{
    G4AccumulableManager::Instance()->RegisterAccumulable(totalEvents);
}

G4String ImpAnalysis::buildFilename(const G4String& pfx)
{
    static const char* OUT_DIR = "data-out";
    using clk = std::chrono::system_clock;

    std::stringstream ss;
    auto nowOut = std::chrono::duration_cast<std::chrono::milliseconds>(
            clk::now().time_since_epoch());

    std::string fidPfx = flareIdentifier.empty() ? "" : (flareIdentifier + "-");
    ss << OUT_DIR << "/" << fidPfx << pfx
       << nowOut.count() << ".csv";
    return ss.str();
}

ImpAnalysis::~ImpAnalysis()
{
    if (anInst) { delete anInst; anInst = nullptr; }
}

ImpAnalysis* ImpAnalysis::instance()
{
    G4AutoLock l(&instMux);
    if (anInst == nullptr) anInst = new ImpAnalysis;
    return anInst;
}

void ImpAnalysis::bookTuplesHistograms(G4bool isMaster)
{
    G4AutoLock l(&dataMux);
    if (!isMaster) return;

    G4AccumulableManager::Instance()->Reset();

    const size_t NFILES = 3;
    std::array<G4String, NFILES> fileNames = {
        IN_SPEC_FN_PFX, OUT_SPEC_FN_PFX, SI_OUT_FN_PFX};
    std::array<std::ofstream*, NFILES> files = {
        &specInFile, &specOutFile, &siOutFile};

    for (size_t i = 0; i < fileNames.size(); ++i) {
        if (files[i]->is_open()) files[i]->close();
        files[i]->open(buildFilename(fileNames[i]));
    }
}

void ImpAnalysis::saveFiles(G4bool isMaster)
{
    G4AutoLock l(&dataMux);
    if (!isMaster) return;

    G4AccumulableManager::Instance()->Merge();
    G4cout << "Total events " << totalEvents.GetValue() << G4endl;

    specOutFile.flush();
    specInFile.flush();
    specOutFile.close();
    specInFile.close();
    siOutFile.flush();
    siOutFile.close();
}

void ImpAnalysis::saveEvent(const G4Event* evt)
{
    G4AutoLock l(&dataMux);
    auto* hcote = evt->GetHCofThisEvent();
    if (hcote == nullptr) return; 
    
    G4int numHitColl = hcote->GetNumberOfCollections();
    for (G4int i = 0; i < numHitColl; ++i) {
        const auto* hc = hcote->GetHC(i);
        if (hc == nullptr) continue;
        processHitCollection(hc);
    }
}

void ImpAnalysis::processHitCollection(const G4VHitsCollection* hc)
{
    if (hc == nullptr) return;

    const auto* concreteHc = static_cast<const G4THitsCollection<ImpVHit>*>(hc);
    const auto* vec = concreteHc->GetVector();
    if (vec == nullptr || vec->size() == 0) return;

    const auto* testHit = (*vec)[0];
    if (testHit->hitType() == ImpVHit::HitType::ScintCryst) {
        saveCrystalHits(vec);
    } else if (testHit->hitType() == ImpVHit::HitType::Si) {
        /* printSiHits(vec); */
        saveSiHits(vec);
    } else
        G4Exception("src/ImpAnalysis.cc processHitCollection", "", FatalException, "unrecognized hit. what?");

    saveIncidentSpectrumChunk();
}

void ImpAnalysis::saveCrystalHits(const std::vector<ImpVHit*>* vec)
{
    std::map<G4String, G4double> deposits;
    for (const auto h : *vec) {
        auto* niceHit = static_cast<ImpScintCrystalHit*>(h);
        const auto& chId = niceHit->peekAssociatedChannelId();
        if (!deposits.contains(niceHit->peekAssociatedChannelId()))
            deposits[chId] = 0;
        auto curEng = niceHit->peekDepositedEnergy();
        /* G4cout << "current energy is " << curEng/keV << " keV" << G4endl; */
        deposits[chId] += curEng;
    }

    // from scattering out of one into another, etc.
    std::size_t crystalsTouched = deposits.size();
    addEvents(crystalsTouched);

    for (const auto& p : deposits)
        specOutFile << (p.second / keV) << '\t' << p.first << std::endl;
}

void ImpAnalysis::addIncidentEnergy(long double e)
{
    G4AutoLock l(&dataMux);
    incidentEnergiesChunk.push_back(e);
}

void ImpAnalysis::saveIncidentSpectrumChunk()
{
    for (const auto& e : incidentEnergiesChunk)
        specInFile << e / keV << std::endl;

    incidentEnergiesChunk.clear();
}

void ImpAnalysis::printSiHits(const std::vector<ImpVHit*>* vec)
{
    G4cout << "there were " << vec->size() << " silicon hits" << G4endl;
    G4cout.flush();
}

void ImpAnalysis::saveSiHits(const std::vector<ImpVHit*>* vec)
{
    siOutFile << vec->size() << std::endl;
    if (savePositions) {
        for (const auto* h : *vec) {
            const auto& pos = h->peekPosition();
            siOutFile << pos.x()/cm << ' ' << pos.y()/cm << ' '
                      << pos.z()/cm << ' ' << h->peekAssociatedChannelId()
                      << std::endl;
        }
    }
}
