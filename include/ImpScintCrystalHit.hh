#pragma once

#include "G4VHit.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"

#include "ImpVHit.hh"

class ImpScintCrystalHit : public ImpVHit
{
    public:
        ImpScintCrystalHit(G4double depositedEnergy, const G4String& channelId, const G4ThreeVector& position);
        ~ImpScintCrystalHit();
        const ImpScintCrystalHit& operator=(const ImpScintCrystalHit& rhs);
        bool operator==(const ImpScintCrystalHit& rhs);

        void* operator new(size_t sz);
        void operator delete(void* toDelete);

        void Print() override;
        HitType hitType() const override
        { return HitType::ScintCryst; }

        G4double peekDepositedEnergy() const override
        { return depositedEnergy; }
        const G4ThreeVector& peekPosition() const override
        { return position; }
        const G4String& peekAssociatedChannelId() const override
        { return assocChannelId; }

    private:
        G4double depositedEnergy;
        G4String assocChannelId;
        G4ThreeVector position;
};
