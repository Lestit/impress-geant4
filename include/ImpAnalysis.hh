#pragma once

#include "G4Accumulable.hh"
#include "G4String.hh"
#include "G4VAnalysisManager.hh"

class G4Event;
class G4Run;
class G4VHitsCollection;

class ImpScintCrystalHit;
class ImpVHit;
/* template<class T> std::vector<T>; */

class ImpAnalysis
{
    public:
        static ImpAnalysis* instance();
        void quit();

        void bookTuplesHistograms(G4bool isMaster);
        void saveFile(G4bool isMaster);
        void saveEvent(const G4Event* evt);
        void addHits(size_t nHits)
        { totalHits += nHits; };

        ImpAnalysis(const ImpAnalysis&) =delete;
        void operator=(const ImpAnalysis&) =delete;
    private:
        ImpAnalysis();
        ~ImpAnalysis();
        G4String buildFilename(const char* pfx);
        void processEvent(const G4Event* evt);
        void processHitCollection(const G4VHitsCollection* hc);
        void saveCrystalHits(const std::vector<ImpVHit*>* vec);

        G4VAnalysisManager* man;
        G4Accumulable<size_t> totalHits;

        G4int energyColId;
        G4int channelColId;

        G4String outFn;
        G4int collectionNumber;
};
