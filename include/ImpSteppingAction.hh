#pragma once

#include <G4UserSteppingAction.hh>

class G4Step;

class ImpSteppingAction : public G4UserSteppingAction
{
    public:
        ImpSteppingAction();
        ~ImpSteppingAction();

        void UserSteppingAction(const G4Step* step) override;

    private:
        void processOptical(const G4Step* step);
        void processDetected(
            const G4VPhysicalVolume* preVol, const G4VPhysicalVolume* postVol,
            const G4Step* step);
};
