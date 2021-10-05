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
    static const G4String FILE_PFX = "imp-";

    static const G4String ENERGY_NTUPLE_NAME = "depetup";
}

ImpAnalysis::ImpAnalysis() :
    totalEvents(0),
    energyColId(-1),
    channelColId(-1),
    collectionNumber(0)
{
    G4AccumulableManager::Instance()->RegisterAccumulable(totalEvents);
}

G4String ImpAnalysis::buildFilename(const char* pfx)
{
    static const char* OUT_DIR = "data-out";
    using clk = std::chrono::system_clock;

    std::stringstream ss;
    auto nowOut = std::chrono::duration_cast<std::chrono::milliseconds>(clk::now().time_since_epoch());
    ss << OUT_DIR << "/" << pfx
       << nowOut.count() << ".csv";
    return ss.str();
}

ImpAnalysis::~ImpAnalysis()
{
    if (anInst) delete anInst;
    anInst = nullptr;
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

    if (outf.is_open()) outf.close();
    outf.open(buildFilename(FILE_PFX));
}

void ImpAnalysis::saveFile(G4bool isMaster)
{
    G4AutoLock l(&dataMux);
    if (!isMaster) return;

    G4AccumulableManager::Instance()->Merge();
    G4cout << "Total events " << totalEvents.GetValue() << G4endl;
    outf.flush();
    outf.close();
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
    if (testHit->hitType() == ImpVHit::HitType::ScintCryst)
        saveCrystalHits(vec);
    else
        G4Exception("src/ImpAnalysis.cc processHitCollection", "", FatalException, "unrecognized hit. what?");
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
        G4cout << "current energy is " << curEng/keV << " keV" << G4endl;
        deposits[chId] += curEng;
    }

    // from scattering out of one into another, etc.
    std::size_t crystalsTouched = deposits.size();
    addEvents(crystalsTouched);

    for (const auto& p : deposits)
        outf << (p.second / keV) << '\t' << p.first << std::endl;
}
