#pragma once

#include <fstream>
#include <unordered_map>

#include "G4Accumulable.hh"
#include "G4String.hh"
#include "G4VAnalysisManager.hh"

class G4Event;
class G4Run;
class G4VHitsCollection;

class ImpScintCrystalHit;
class ImpVHit;

class ImpAnalysis
{
    public:
        static ImpAnalysis* instance();

        void bookTuplesHistograms(G4bool isMaster);
        void saveFile(G4bool isMaster);
        void saveEvent(const G4Event* evt);
        void addEvents(std::size_t nEvts)
        { totalEvents += nEvts; };

        ImpAnalysis(const ImpAnalysis&) =delete;
        void operator=(const ImpAnalysis&) =delete;
    private:
        ImpAnalysis();
        ~ImpAnalysis();
        G4String buildFilename(const char* pfx);
        void processEvent(const G4Event* evt);
        void processHitCollection(const G4VHitsCollection* hc);
        void saveCrystalHits(const std::vector<ImpVHit*>* vec);

        G4Accumulable<std::size_t> totalEvents;

        G4int energyColId;
        G4int channelColId;

        G4String outFn;
        G4int collectionNumber;

        std::ofstream outf;
};
