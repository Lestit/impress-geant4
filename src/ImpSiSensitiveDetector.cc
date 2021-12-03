#include <G4HCofThisEvent.hh>
#include <G4OpticalPhoton.hh>
#include <G4SDManager.hh>
#include <G4Step.hh>
#include <G4THitsCollection.hh>
#include <G4TouchableHistory.hh>

#include <ImpSiHit.hh>
#include <ImpSiSensitiveDetector.hh>

namespace {
    static const G4String IMP_SI_SENS_DET_SUFFIX = "_si_sens_det";
}

ImpSiSensitiveDetector::
ImpSiSensitiveDetector(const G4String& detectorName, const G4String& channelId) :
    G4VSensitiveDetector(detectorName),
    channelId(channelId),
    thisCollectionName(detectorName + IMP_SI_SENS_DET_SUFFIX),
    hitsCollectionId(-1)
{
    collectionName.insert(thisCollectionName);
}

ImpSiSensitiveDetector::~ImpSiSensitiveDetector()
{ }

void ImpSiSensitiveDetector::
Initialize(G4HCofThisEvent* hcote)
{
    // FIXME: copied from the scintillator class. maybe we should just subclass? ugh.
    // polymorphism in the hitscollection
    hitsCollection = new G4THitsCollection<ImpVHit>(
        SensitiveDetectorName, thisCollectionName);
    hitsCollectionId = G4SDManager::GetSDMpointer()->GetCollectionID(thisCollectionName);

    hcote->AddHitsCollection(hitsCollectionId, hitsCollection);
}

G4bool ImpSiSensitiveDetector::
ProcessHits(G4Step* step, G4TouchableHistory*)
{
    /* G4cout << "*** in optical" << G4endl; */
    // with 100% efficiency hits don't really register...
    /* if (step->GetTrack()->GetParticleDefinition() == G4OpticalPhoton::Definition()) */
        /* processOptical(step); */

    return false;
}

void ImpSiSensitiveDetector::
processOptical(const G4Step* step)
{
    /* G4cout << "*** in optical" << G4endl; */
    auto edep = step->GetTotalEnergyDeposit();
    if (edep == 0)
        return;

    auto pos = step->GetPostStepPoint()->GetPosition(); //(step->GetPreStepPoint()->GetPosition() + step->GetPostStepPoint()->GetPosition()) / 2;
    auto* hit = new ImpSiHit(channelId, pos, edep);
    hitsCollection->insert(hit);
}
