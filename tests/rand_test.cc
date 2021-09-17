#include <cmath>
#include <cassert>
#include <iostream>
#include <sstream>

#include <G4Box.hh>
#include <G4PhysicalConstants.hh>
#include <G4Tubs.hh>
#include <G4ThreeVector.hh>
#include <G4Types.hh>

#include <ImpSurfacePointPicker.hh>

static const size_t TRIALS = 100000;

void testBox(ssize_t seed)
{
    double radius = 3;
    double depth = 1;
    auto* c = new G4Tubs("test-tubs", 0, radius, depth/2, 0, 2*pi);
    ImpSurfacePointPicker spp(c, seed);

    for (size_t i = 0; i < TRIALS; ++i) {
        auto v = spp.pickPoint();
        bool radCrit = sqrt(v.x()*v.x() + v.y()*v.y()) <= radius;
        bool zCrit = v.z() == depth/2;
        if (!radCrit || !zCrit) throw std::runtime_error("Cylinder issue");
    }
}

void testCyl(ssize_t seed)
{
    double xHalfLen = 1, yHalfLen = 1, depth = 1;
    auto* b = new G4Box("test-box", xHalfLen, yHalfLen, depth/2);
    ImpSurfacePointPicker spp(b, seed);

    for (size_t i = 0; i < TRIALS; ++i) {
        auto v = spp.pickPoint();
        bool horizCrit = (abs(v.x()) <= xHalfLen && abs(v.y()) <= yHalfLen);
        bool zCrit = (v.z() == depth/2);
        if (!zCrit) {
            std::cout << "v.z() is " << v.z() << std::endl
                      << "depth/2 is" << depth/2 << std::endl;
            throw std::runtime_error("Box z issue");
        }
        if (!horizCrit) {
            throw std::runtime_error("Box horizontal issue");
        }
    }
    
}

int main(int argc, char* argv[])
{
    ssize_t seed = ImpSurfacePointPicker::NOSEED;
    if (argc > 1) {
        std::stringstream ss(argv[1]);
        ss >> seed;
    }

    testCyl(seed);
    testBox(seed);

    return 0;
}
