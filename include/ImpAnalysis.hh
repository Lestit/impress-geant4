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

class ImpAnalysisFileWrapper
{
    public:
        ImpAnalysisFileWrapper(
            const G4String& prefix, bool isBinary, const G4String& flareId="");
        ~ImpAnalysisFileWrapper();

        std::ofstream& file()
        { return outf; }

        void reset(std::uint64_t newTimePfx);
        void updateFlareId(const G4String& fid);
    private:
        G4String buildFilename();
        std::uint64_t timePfx;
        bool isBinary;
        G4String fileNamePrefix;
        G4String flareId;
        std::ofstream outf;
};

class ImpAnalysis
{
    public:
        static ImpAnalysis* instance();

        void initFiles(G4bool isMaster);
        void saveFiles(G4bool isMaster);
        void saveEvent(const G4Event* evt);

        void addEvents(std::size_t nEvts)
        { totalEvents += nEvts; };

        void addIncidentEnergy(long double e);
        void saveScintillated(std::size_t num);

        const std::string& peekFlareIdentifier()
        { return flareIdentifier; }

        void updateFlareIdentifier(const std::string& fid);

        ImpAnalysis(const ImpAnalysis&) =delete;
        void operator=(const ImpAnalysis&) =delete;
    private:
        ImpAnalysis();
        ~ImpAnalysis();
        void processEvent(const G4Event* evt);
        void processHitCollection(const G4VHitsCollection* hc);
        void saveCrystalHits(const std::vector<ImpVHit*>* vec);
        void saveIncidentSpectrumChunk();

        void printSiHits(const std::vector<ImpVHit*>* vec);
        void saveSiHits(const std::vector<ImpVHit*>* vec);

        G4Accumulable<std::size_t> totalEvents;
        bool saveSiEnergies;
        bool saveSiPositions;

        std::vector<long double> incidentEnergiesChunk;

        std::string flareIdentifier;
        ImpAnalysisFileWrapper crystOut;
        ImpAnalysisFileWrapper specIn;
        ImpAnalysisFileWrapper siOut;
        ImpAnalysisFileWrapper siEngOut;
        ImpAnalysisFileWrapper scintOut;
        std::vector<ImpAnalysisFileWrapper*> wrappers;
};
