#pragma once

#include <G4String.hh>
#include <G4VSensitiveDetector.hh>

#include <ImpVHit.hh>

class G4HCofThisEvent;
class G4Step;
class G4TouchableHistory;
template <class T> class G4THitsCollection;

class ImpSiSensitiveDetector : public G4VSensitiveDetector
{
    public:
        ImpSiSensitiveDetector(const G4String& detectorName, const G4String& channelId);
        ~ImpSiSensitiveDetector();

        void Initialize(G4HCofThisEvent* hcote) override;
        G4bool ProcessHits(G4Step* step, G4TouchableHistory*) override;
        void processOptical(const G4Step* step);

    private:
        G4String channelId;
        G4String thisCollectionName;
        G4THitsCollection<ImpVHit>* hitsCollection;
        G4int hitsCollectionId;
};
