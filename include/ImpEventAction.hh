#pragma once

#include "G4UserEventAction.hh"

class G4Event;
class G4TrajectoryContainer;

class ImpEventAction : public G4UserEventAction
{
    public:
        ImpEventAction();
        ~ImpEventAction();

        void doKeepAllEvents(bool yesno)
        { keepAllEvents = yesno; }

        void BeginOfEventAction(const G4Event* evt) override;
        void EndOfEventAction(const G4Event* evt) override;
    private:
        bool keepAllEvents;
        void sampleDrawOptical(G4TrajectoryContainer* trjCon);
        void printHits(const G4Event* evt);
};
