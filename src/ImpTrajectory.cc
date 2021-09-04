#include "G4Allocator.hh"

#include "ImpTrajectory.hh"

namespace {
    G4ThreadLocal G4Allocator<ImpTrajectory>* trjAlloc = nullptr;
}

ImpTrajectory::ImpTrajectory(const G4Track* t) :
    G4Trajectory(t),
    draw(true)
{ }

ImpTrajectory::~ImpTrajectory()
{ }

void ImpTrajectory::DrawTrajectory() const
{ if (draw) G4Trajectory::DrawTrajectory(); }

void* ImpTrajectory::operator new(size_t)
{
    if (trjAlloc == nullptr) trjAlloc = new G4Allocator<ImpTrajectory>;
    return static_cast<void*>(trjAlloc->MallocSingle());
}

void ImpTrajectory::operator delete(void* d)
{
    trjAlloc->FreeSingle(static_cast<ImpTrajectory*>(d));
}
