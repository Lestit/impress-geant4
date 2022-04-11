#pragma once

#include "G4VHit.hh"
#include "G4ThreeVector.hh"

class ImpVHit : public G4VHit
{
    public:
        ImpVHit(const G4String& channelId, const G4ThreeVector& position, G4double arrivalTime);
        ~ImpVHit() =0;
        enum class HitType { ScintCryst, Si };

        virtual HitType hitType() const =0;
        virtual const G4ThreeVector& peekPosition() const
        { return position; }
        virtual const G4String& peekAssociatedChannelId() const
        { return assocChannelId; }
        virtual G4double peekArrivalTime() const
        { return arrivalTime; }

        virtual void Draw() override;

    protected:
        G4String assocChannelId;
        G4double arrivalTime;
        G4ThreeVector position;
};
