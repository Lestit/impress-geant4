#pragma once

#include "G4UserEventAction.hh"

class G4Event;
class G4TrajectoryContainer;

class ImpEventAction : public G4UserEventAction
{
    public:
        ImpEventAction();
        ~ImpEventAction();

        void BeginOfEventAction(const G4Event* evt) override;
        void EndOfEventAction(const G4Event* evt) override;
    private:
        void sampleDrawOptical(G4TrajectoryContainer* trjCon);
        void printHits(const G4Event* evt);
};
