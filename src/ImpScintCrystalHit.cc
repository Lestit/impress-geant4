#include "G4Allocator.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "ImpScintCrystalHit.hh"

namespace {
    G4ThreadLocal G4Allocator<ImpScintCrystalHit>* scintHitAllocator = nullptr;
}

ImpScintCrystalHit::ImpScintCrystalHit(
        G4double depositedEnergy, const G4String& channelId, const G4ThreeVector& position) :
    G4VHit(),
    depositedEnergy(depositedEnergy),
    assocChannelId(channelId),
    position(position)
{ }

ImpScintCrystalHit::~ImpScintCrystalHit()
{ }

void ImpScintCrystalHit::Print()
{
    auto f = G4cout.flags();
    G4cout
        << "=== Begin ScintCrystalHit ===" << G4endl
        << "Energy deposited: " << std::setw(7) << G4BestUnit(depositedEnergy, "Energy") << G4endl
        << "Position:         " << std::setw(7) << G4BestUnit(position, "Length") << G4endl
        << "Channel ID:       " << std::setw(7) << assocChannelId << G4endl
        << "=== End ScintCrystalHit ===" << G4endl;
    G4cout.flags(f);
    G4cout.flush();
}

bool ImpScintCrystalHit::operator==(const ImpScintCrystalHit& rhs)
{
    return
        depositedEnergy == rhs.peekDepositedEnergy() &&
        position == rhs.peekPosition() &&
        assocChannelId == rhs.peekAssociatedChannelId();
}

const ImpScintCrystalHit& ImpScintCrystalHit::operator=(const ImpScintCrystalHit& rhs)
{
    depositedEnergy = rhs.peekDepositedEnergy();
    position = rhs.peekPosition();
    assocChannelId = rhs.peekAssociatedChannelId();
    return *this;
}

void* ImpScintCrystalHit::operator new(size_t)
{
    if (!scintHitAllocator) scintHitAllocator = new G4Allocator<ImpScintCrystalHit>;
    return static_cast<void*>(scintHitAllocator->MallocSingle());
}

void ImpScintCrystalHit::operator delete(void* toDelete)
{
    scintHitAllocator->FreeSingle(static_cast<ImpScintCrystalHit*>(toDelete));
}
