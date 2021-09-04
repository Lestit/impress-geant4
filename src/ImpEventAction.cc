#include "G4Event.hh"
#include "G4THitsCollection.hh"
#include "G4ThreeVector.hh"
#include "G4VVismanager.hh"
#include "G4VTrajectory.hh"

#include "ImpEventAction.hh"
#include "ImpScintCrystalHit.hh"

ImpEventAction::ImpEventAction() :
    G4UserEventAction()
{ }

ImpEventAction::~ImpEventAction()
{ }

void ImpEventAction::BeginOfEventAction(const G4Event* evt)
{ G4UserEventAction::BeginOfEventAction(evt); }

void ImpEventAction::EndOfEventAction(const G4Event* evt)
{
    printHits(evt);
    if (G4VVisManager::GetConcreteInstance()) {
        sampleDrawOptical(evt->GetTrajectoryContainer());
    }
}

void ImpEventAction::sampleDrawOptical(G4TrajectoryContainer* trjCon)
{
    G4int i = 0, rest = 1000;
    for (G4VTrajectory* trj : *trjCon->GetVector()) {
        if (trj->GetParticleName() == "opticalphoton") {
            if (i == 0) trj->DrawTrajectory();
            i = (i + 1) % rest;
        }
    }
}

void ImpEventAction::printHits(const G4Event* evt)
{
    auto* hcote = evt->GetHCofThisEvent();
    if (hcote == nullptr) return;

    for (G4int i = 0; i < hcote->GetNumberOfCollections(); ++i) {
        auto* hc = hcote->GetHC(i);
        if (hc) hc->PrintAllHits();
    }
}
