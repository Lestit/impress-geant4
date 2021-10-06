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

        void addIncidentEnergy(long double e);

        ImpAnalysis(const ImpAnalysis&) =delete;
        void operator=(const ImpAnalysis&) =delete;
    private:
        ImpAnalysis();
        ~ImpAnalysis();
        G4String buildFilename(const char* pfx);
        void processEvent(const G4Event* evt);
        void processHitCollection(const G4VHitsCollection* hc);
        void saveCrystalHits(const std::vector<ImpVHit*>* vec);
        void saveIncidentSpectrumChunk();

        G4Accumulable<std::size_t> totalEvents;

        std::vector<long double> incidentEnergiesChunk;

        std::ofstream specOutFile;
        std::ofstream specInFile;
};
