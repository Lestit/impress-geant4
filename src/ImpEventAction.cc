#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4THitsCollection.hh"
#include "G4ThreeVector.hh"
#include "G4VVismanager.hh"
#include "G4VTrajectory.hh"

#include "ImpAnalysis.hh"
#include "ImpEventAction.hh"
#include "ImpScintCrystalHit.hh"
#include "ImpTrajectory.hh"
#include "ImpVHit.hh"

ImpEventAction::ImpEventAction() :
    G4UserEventAction(),
    keepAllEvents(false)
{ }

ImpEventAction::~ImpEventAction()
{ }

void ImpEventAction::BeginOfEventAction(const G4Event*)
{ }

void ImpEventAction::EndOfEventAction(const G4Event* evt)
{
    auto* hcote = evt->GetHCofThisEvent();
    size_t nHitColls = 0;
    if (hcote) {
        for (auto i = 0; i < hcote->GetNumberOfCollections(); ++i) {
            nHitColls += hcote->GetHC(i)->GetSize();
        }
    }

    bool keepIt = keepAllEvents || (hcote && nHitColls > 0);
    if (keepIt) ImpAnalysis::instance()->saveEvent(evt);

    if (G4VVisManager::GetConcreteInstance()) {
        auto* trjCon = evt->GetTrajectoryContainer();
        for (G4VTrajectory* trj : *(trjCon->GetVector())) {
            static_cast<ImpTrajectory*>(trj)->doDraw(keepIt);
        }

        sampleDrawOptical(trjCon);
    }
}

void ImpEventAction::sampleDrawOptical(G4TrajectoryContainer* trjCon)
{
    // only draw every #(rest) optical photons
    G4int i = 0, rest = 10;
    for (G4VTrajectory* trj : *trjCon->GetVector()) {
        auto* impTrj = static_cast<ImpTrajectory*>(trj);
        if (impTrj->GetParticleName() == "opticalphoton") {
            if (i != 0) impTrj->doDraw(false);
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
