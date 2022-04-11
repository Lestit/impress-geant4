#include <G4Allocator.hh>
#include <G4UnitsTable.hh>
#include <G4SystemOfUnits.hh>

#include <ImpSiHit.hh>

namespace
{
    G4ThreadLocal G4Allocator<ImpSiHit>* alloc = nullptr;
}

ImpSiHit::ImpSiHit(
    const G4String& channelId, const G4ThreeVector& position, G4double arrivalTime, G4double depositedEnergy) :
        ImpVHit(channelId, position, arrivalTime), depositedEnergy(depositedEnergy)
{ }

ImpSiHit::~ImpSiHit()
{ }

void ImpSiHit::Print()
{
    auto f = G4cout.flags();
    G4cout
        << "=== Begin SiCrystalHit ===" << G4endl
        << "Energy deposited: " << std::setw(7) << G4BestUnit(depositedEnergy, "Energy") << G4endl
        << "Position:         " << std::setw(7) << G4BestUnit(position, "Length") << G4endl
        << "Arrival time:     " << std::setw(7) << G4BestUnit(arrivalTime, "Time") << G4endl
        << "Channel ID:       " << std::setw(7) << assocChannelId << G4endl
        << "=== End SiCrystalHit ===" << G4endl;
    G4cout.flags(f);
    G4cout.flush();
}

bool ImpSiHit::operator==(const ImpSiHit& rhs)
{
    return
        position == rhs.peekPosition() &&
        assocChannelId == rhs.peekAssociatedChannelId() &&
        arrivalTime == rhs.peekArrivalTime() &&
        depositedEnergy == rhs.peekDepositedEnergy();
}

const ImpSiHit& ImpSiHit::operator=(const ImpSiHit& rhs)
{
    position = rhs.peekPosition();
    assocChannelId = rhs.peekAssociatedChannelId();
    arrivalTime = rhs.peekArrivalTime();
    depositedEnergy = rhs.peekDepositedEnergy();
    return *this;
}

void* ImpSiHit::operator new(size_t)
{
    if (!alloc) alloc = new G4Allocator<ImpSiHit>;
    return static_cast<void*>(alloc->MallocSingle());
}

void ImpSiHit::operator delete(void* toDelete)
{
    alloc->FreeSingle(static_cast<ImpSiHit*>(toDelete));
}
