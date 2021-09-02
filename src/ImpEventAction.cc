#include "ImpEventAction.hh"

ImpEventAction::ImpEventAction() :
    G4UserEventAction()
{ }

ImpEventAction::~ImpEventAction()
{ }

void ImpEventAction::BeginOfEventAction(const G4Event* evt)
{ G4UserEventAction::BeginOfEventAction(evt); }

void ImpEventAction::EndOfEventAction(const G4Event* evt)
{
    if (G4VVisManager::GetConcreteInstance()) {
        forceDrawOptical(evt->GetTrajectoryContainer());
    }

    G4UserEventAction::EndOfEventAction(evt);
}

void ImpEventAction::forceDrawOptical(G4TrajectoryContainer* trjCon)
{
    for (G4VTrajectory* trj : *trjCon->GetVector()) {
        if (trj->GetParticleName() == "opticalphoton") {
            trj->DrawTrajectory();
        }
    }
}
