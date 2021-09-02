#pragma once

#include "G4Event.hh"
#include "G4ThreeVector.hh"
#include "G4UserEventAction.hh"
#include "G4VVismanager.hh"
#include "G4VTrajectory.hh"

class ImpEventAction : public G4UserEventAction
{
    public:
        ImpEventAction();
        ~ImpEventAction();

        void BeginOfEventAction(const G4Event* evt) override;
        void EndOfEventAction(const G4Event* evt) override;
    private:
        void forceDrawOptical(G4TrajectoryContainer* trjCon);
};
