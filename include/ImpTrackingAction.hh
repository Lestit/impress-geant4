#pragma once

#include "G4UserTrackingAction.hh"

class ImpTrackingAction : public G4UserTrackingAction
{
    public:
        ImpTrackingAction();
        ~ImpTrackingAction();

        void PreUserTrackingAction(const G4Track* t) override;
};
