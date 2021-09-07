#include <ctime>
#include <chrono>
#include <iomanip>
#include <memory>
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

    static const G4String MANAGER_TYPE = "root";
    static const G4String HISTO_DIRECTORY = "impress-histo";
    static const G4String TUPLE_DIRECTORY = "impress-tuple";
    static const G4String FILE_PFX = "imp-";

    static const G4String ENERGY_NTUPLE_NAME = "depetup";
}

ImpAnalysis::ImpAnalysis() :
    man(G4Analysis::ManagerInstance(MANAGER_TYPE)),
    totalHits(0),
    energyColId(-1),
    channelColId(-1),
    collectionNumber(0)
{
    man->SetNtupleDirectoryName(TUPLE_DIRECTORY);
    man->SetHistoDirectoryName(HISTO_DIRECTORY);
    outFn = buildFilename(FILE_PFX);
    man->OpenFile(outFn);
    
    G4AccumulableManager::Instance()->RegisterAccumulable(totalHits);
}

G4String ImpAnalysis::buildFilename(const char* pfx)
{
    static const char* TIME_FORMAT = "%F_%H:%M:%S";
    // year-month-day-hour:min:sec
    using clk = std::chrono::system_clock;

    time_t timeNow = clk::to_time_t(clk::now());
    std::tm* nowTm = std::localtime(&timeNow);
    std::stringstream ss;
    ss << pfx << std::put_time(nowTm, TIME_FORMAT);
    return ss.str();
}

ImpAnalysis::~ImpAnalysis()
{
    if (man) delete man;
    man = nullptr;
    if (anInst) delete anInst;
    anInst = nullptr;
    G4cout << "********************************88DESTRUCTOR" << G4endl;
}

ImpAnalysis* ImpAnalysis::instance()
{
    G4AutoLock l(&instMux);
    if (anInst == nullptr) anInst = new ImpAnalysis;
    return anInst;
}

void ImpAnalysis::quit()
{
    /* G4AutoLock l(&instMux); */
    /* if (man) delete man; */
    /* man = nullptr; */
    /* if (anInst) delete anInst; */
    /* anInst = nullptr; */
}

void ImpAnalysis::bookTuplesHistograms(G4bool isMaster)
{
    G4AutoLock l(&dataMux);
    if (!isMaster) return;

    G4AccumulableManager::Instance()->Reset();

    man->CreateNtuple(ENERGY_NTUPLE_NAME, "Energy deposited and channel ID");
    energyColId = man->CreateNtupleDColumn("Edep");
    channelColId = man->CreateNtupleDColumn("ChannelId");
    man->FinishNtuple();
}

void ImpAnalysis::saveFile(G4bool isMaster)
{
    G4AutoLock l(&dataMux);
    if (!isMaster) return;

    G4AccumulableManager::Instance()->Merge();
    G4cout << "Total hits " << totalHits.GetValue() << G4endl;
    man->Write();
    man->CloseFile();
}

void ImpAnalysis::saveEvent(const G4Event* evt)
{
    G4AutoLock l(&dataMux);
    auto* hcote = evt->GetHCofThisEvent();
    if (hcote == nullptr) return; 
    
    G4int numHitColl = hcote->GetNumberOfCollections();
    for (G4int i = 0; i < numHitColl; ++i) {
        auto* hc = hcote->GetHC(i);
        if (hc == nullptr) continue;
        processHitCollection(hc);
        addHits(hc->GetSize());
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
    G4double totalEnergy = 0;
    for (const auto h : *vec) {
        auto* niceHit = static_cast<ImpScintCrystalHit*>(h);
        totalEnergy += niceHit->peekDepositedEnergy();
    }

    /* if (totalEnergy > 0) { */
    man->FillNtupleDColumn(energyColId, totalEnergy / keV);
    man->FillNtupleDColumn(channelColId, -1);
    man->AddNtupleRow();
    /* } */
}
