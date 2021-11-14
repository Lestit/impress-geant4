#pragma once

#include <G4String.hh>
#include <G4ThreeVector.hh>

#include <ImpVHit.hh>

class ImpSiHit : public ImpVHit
{
    public:
        ImpSiHit(const G4String& channelId, const G4ThreeVector& position);
        ~ImpSiHit();
        const ImpSiHit& operator=(const ImpSiHit& rhs);
        bool operator==(const ImpSiHit& rhs);

        void* operator new(size_t sz);
        void operator delete(void* toDelete);

        void Print() override;
        HitType hitType() const override
        { return HitType::Si; }
};
