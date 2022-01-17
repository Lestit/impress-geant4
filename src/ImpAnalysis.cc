#include <ctime>
#include <chrono>
#include <filesystem>
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
#include "ImpSiHit.hh"
#include "ImpVHit.hh"

namespace {
    ImpAnalysis *anInst = nullptr;
    G4Mutex instMux = G4MUTEX_INITIALIZER;
    G4Mutex dataMux = G4MUTEX_INITIALIZER;

    static const G4String kSPEC_IN = "spec-in";
    static const G4String kCRYST_OUT = "cryst-out";
    static const G4String kSI_OUT = "si-out";
    static const G4String kSCINT_OUT = "scint-phots-out";
    static const G4String kSI_ENGS_OUT = "si-energies-out";
    static const std::string OUT_DIR = "data-out";
}

static std::string genBaseSubfolder(std::uint64_t milliz);

ImpAnalysis::ImpAnalysis() :
    totalEvents(0),
    saveSiEnergies(false),
    saveSiPositions(true),
    crystOut(kCRYST_OUT, false),
    specIn(kSPEC_IN, false),
    siOut(kSI_OUT, false),
    siEngOut(kSI_ENGS_OUT, true),
    scintOut(kSCINT_OUT, false)
{
    G4AccumulableManager::Instance()->RegisterAccumulable(totalEvents);
    wrappers = {&crystOut, &specIn, &siOut, &siEngOut, &scintOut};
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

void ImpAnalysis::updateFlareIdentifier(const std::string& fid)
{
    flareIdentifier = fid;
    for (const auto w : wrappers)
        w->updateFlareId(fid);
}

void ImpAnalysis::initFiles(G4bool isMaster)
{
    G4AutoLock l(&dataMux);
    if (!isMaster) return;

    G4AccumulableManager::Instance()->Reset();

    using clk = std::chrono::system_clock;
    auto nowOut = std::chrono::duration_cast<std::chrono::milliseconds>(
        clk::now().time_since_epoch());
    auto cnt = nowOut.count();

    for (auto* w : wrappers) {
        w->reset(cnt);
    }
}

void ImpAnalysis::saveFiles(G4bool isMaster)
{
    G4AutoLock l(&dataMux);
    if (!isMaster) return;

    G4AccumulableManager::Instance()->Merge();
    G4cout << "Total events " << totalEvents.GetValue() << G4endl;

    for (auto& w : wrappers) {
        w->file().flush();
        w->file().close();
    }
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
        saveSiHits(vec);
    } else
        G4Exception(
            "src/ImpAnalysis.cc processHitCollection",
            "", FatalException, "unrecognized hit. what?");

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
        deposits[chId] += curEng;
    }

    // from scattering out of one into another, etc.
    std::size_t crystalsTouched = deposits.size();
    addEvents(crystalsTouched);

    for (const auto& p : deposits)
        crystOut.file() << (p.second / keV) << '\t' << p.first << std::endl;
}

void ImpAnalysis::addIncidentEnergy(long double e)
{
    G4AutoLock l(&dataMux);
    incidentEnergiesChunk.push_back(e);
}

void ImpAnalysis::saveIncidentSpectrumChunk()
{
    for (const auto& e : incidentEnergiesChunk)
        specIn.file() << e / keV << std::endl;

    incidentEnergiesChunk.clear();
}

void ImpAnalysis::printSiHits(const std::vector<ImpVHit*>* vec)
{
    G4cout << "there were " << vec->size() << " silicon hits" << G4endl;
    G4cout.flush();
}

void ImpAnalysis::saveSiHits(const std::vector<ImpVHit*>* vec)
{
    siOut.file() << vec->size();
    if (saveSiPositions) {
        for (const auto* h : *vec) {
            siOut.file() << ' ' << h->peekPosition();
        }
    }
    siOut.file() << std::endl;

    if (saveSiEnergies) {
        const size_t sz = vec->size();
        siEngOut.file().write(
            reinterpret_cast<const char*>(&sz),
            sizeof(size_t));
        for (const auto* h : *vec) {
            const auto* nice = static_cast<const ImpSiHit*>(h);
            const double e = nice->peekDepositedEnergy() / eV;
            siEngOut.file().write(
                reinterpret_cast<const char*>(&e),
                sizeof(double));
        }
    }
}

void ImpAnalysis::saveScintillated(std::size_t num)
{
    G4AutoLock l(&dataMux);
    if (num == 0) return;
    // # scintillated per event can vary
    scintOut.file() << num << std::endl;
}

// filewrapper below

ImpAnalysisFileWrapper::ImpAnalysisFileWrapper(
        const G4String& fnPfx, bool isBinary, const G4String& flareId) :
    isBinary(isBinary),
    fileNamePrefix(fnPfx),
    flareId(flareId)
{ }

ImpAnalysisFileWrapper::~ImpAnalysisFileWrapper()
{ }

void ImpAnalysisFileWrapper::updateFlareId(const G4String& fid)
{ flareId = fid; }

void ImpAnalysisFileWrapper::reset(std::uint64_t newTimePfx)
{
    outf.flush();
    outf.close();
    if (newTimePfx) timePfx = newTimePfx;
    auto fn = buildFilename();
    auto openMode = isBinary? std::ios::out : (std::ios::out | std::ios::binary);
    outf.open(fn, openMode);
}

G4String ImpAnalysisFileWrapper::buildFilename()
{
    auto fold = genBaseSubfolder(timePfx);
    std::filesystem::path outPath(OUT_DIR);
    auto p = outPath / fold;
    if (!std::filesystem::exists(p))
        std::filesystem::create_directory(outPath / fold);

    std::stringstream ss;
    std::string fidPfx = flareId.empty() ? "" : (flareId + "-");
    ss << p.string() << "/" << fidPfx << fileNamePrefix << (isBinary? ".bin" : ".tab");

    auto str = ss.str();
    G4cout << "FILENAME IS " << str << G4endl;
    return str;
}

static std::string genBaseSubfolder(std::uint64_t milliz)
{
    static const size_t TIME_FMT_LEN = 256;
    static std::array<char, TIME_FMT_LEN> tmFmtAry;

    std::time_t secondz = milliz / 1e3;
    std::strftime(tmFmtAry.data(), TIME_FMT_LEN, "%F-%T", std::localtime(&secondz));
    if (errno > 0) G4cerr << "Error: " << strerror(errno) << G4endl;
    const char* d = tmFmtAry.data();
    return std::string(d);
}
