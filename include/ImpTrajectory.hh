#pragma once

#include "G4Trajectory.hh"

class ImpTrajectory : public G4Trajectory
{
    public:
        ImpTrajectory(const G4Track* t);
        ~ImpTrajectory();

        void DrawTrajectory() const override;

        void* operator new(size_t sz);
        void operator delete(void* d);
        
        void doDraw(G4bool d)
        { draw = d; }

    private:
        bool draw;
};
