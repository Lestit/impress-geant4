#pragma once

#include "G4String.hh"
#include "G4VSensitiveDetector.hh"

#include "ImpScintCrystalHit.hh"
#include "ImpVHit.hh"

class G4HCofThisEvent;
class G4Step;
class G4TouchableHistory;
template <class T> class G4THitsCollection;

class ImpScintCrystalSensitiveDetector : public G4VSensitiveDetector
{
    public:
        ImpScintCrystalSensitiveDetector(const G4String& detectorName, const G4String& channelId);
        ~ImpScintCrystalSensitiveDetector();

        void Initialize(G4HCofThisEvent* hc) override;
        G4bool ProcessHits(G4Step* step, G4TouchableHistory*) override;
        
    private:
        G4String channelId;
        G4String thisCollectionName; 
        G4THitsCollection<ImpVHit>* hitsCollection;
        G4int hitsCollectionId;
};
