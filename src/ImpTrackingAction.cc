#include "G4Track.hh"
#include "G4TrackingManager.hh"

#include "ImpTrackingAction.hh"
#include "ImpTrajectory.hh"

ImpTrackingAction::ImpTrackingAction() :
    G4UserTrackingAction()
{ }

ImpTrackingAction::~ImpTrackingAction()
{ }

void ImpTrackingAction::PreUserTrackingAction(const G4Track* t)
{
    fpTrackingManager->SetTrajectory(new ImpTrajectory(t));
    G4UserTrackingAction::PreUserTrackingAction(t);
}
