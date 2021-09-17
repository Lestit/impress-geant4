#pragma once

#include <random>

#include <G4ThreeVector.hh>
#include <G4Types.hh>

class G4Tubs;
class G4Box;

class ImpSurfacePointPicker
{
    public:
        static const ssize_t NOSEED = 0xdeadbeef;
        ImpSurfacePointPicker() =delete;
        ImpSurfacePointPicker(const G4Tubs* const worldCyl, ssize_t seed = NOSEED);
        ImpSurfacePointPicker(const G4Box* const worldBox, ssize_t seed = NOSEED);
        ~ImpSurfacePointPicker();
        
        G4ThreeVector pickPoint();

    private:
        std::mt19937_64 rng;
        std::uniform_real_distribution<double> realDis;

        enum class WhichShape { cyl, box };
        const WhichShape which;
        const G4double radius;
        const G4double zOffset;
        const G4double xLen;
        const G4double yLen;

        G4ThreeVector pickCylinder();
        G4ThreeVector pickBox();
};
