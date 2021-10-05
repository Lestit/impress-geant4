#pragma once

#include "G4VHit.hh"
#include "G4ThreeVector.hh"

class ImpVHit : public G4VHit
{
    public:
        enum class HitType { ScintCryst, Sipm };

        virtual HitType hitType() const =0;
        virtual const G4ThreeVector& peekPosition() const =0;
        virtual const G4String& peekAssociatedChannelId() const =0;

        virtual void Draw() override;
};
