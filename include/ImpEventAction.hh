#pragma once

#include <G4UserEventAction.hh>
#include <G4Cache.hh>

class G4Event;
class G4TrajectoryContainer;

class ImpEventAction : public G4UserEventAction
{
    public:
        ImpEventAction();
        ~ImpEventAction();

        void addScintPhot(std::size_t numToAdd)
        { scintPhotPerEvent.Put(scintPhotPerEvent.Get() + numToAdd); }
        void doKeepAllEvents(bool yesno)
        { keepAllEvents = yesno; }

        void BeginOfEventAction(const G4Event* evt) override;
        void EndOfEventAction(const G4Event* evt) override;
    private:
        bool keepAllEvents;
        void sampleDrawOptical(G4TrajectoryContainer* trjCon);
        void printHits(const G4Event* evt);
        G4Cache<std::size_t> scintPhotPerEvent;
};
