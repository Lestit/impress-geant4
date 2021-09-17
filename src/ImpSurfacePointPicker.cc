#include <complex>

#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4PhysicalConstants.hh>

#include <ImpSurfacePointPicker.hh>

ImpSurfacePointPicker::ImpSurfacePointPicker(const G4Tubs* const worldCyl, ssize_t seed) :
    rng(seed == NOSEED? std::random_device()() : seed),
    realDis(0, 1),
    which(WhichShape::cyl),
    radius(worldCyl->GetOuterRadius()),
    zOffset(worldCyl->GetZHalfLength()),
    xLen(-1), yLen(-1)
{ }

ImpSurfacePointPicker::ImpSurfacePointPicker(const G4Box* const worldBox, ssize_t seed) :
    rng(seed == NOSEED? std::random_device()() : seed),
    realDis(0, 1),
    which(WhichShape::box),
    radius(-1),
    zOffset(worldBox->GetZHalfLength()),
    xLen(2 * worldBox->GetXHalfLength()),
    yLen(2 * worldBox->GetYHalfLength())
{ }

ImpSurfacePointPicker::~ImpSurfacePointPicker()
{ }

G4ThreeVector ImpSurfacePointPicker::pickPoint()
{
    if (which == WhichShape::cyl) return pickCylinder();
    else if (which == WhichShape::box) return pickBox();
    
    G4Exception("src/ImpSurfacePointPicker.cc", "", RunMustBeAborted, "no surface present to pick a point from");
    return G4ThreeVector();
}

G4ThreeVector ImpSurfacePointPicker::pickCylinder()
{
    const G4double angle = 2 * pi * realDis(rng);
    const G4double r = radius * realDis(rng);

    // std::cout << r << '\t' << angle << std::endl;
    return G4ThreeVector(r * std::cos(angle), r * std::sin(angle), zOffset);
}

G4ThreeVector ImpSurfacePointPicker::pickBox()
{
    const G4double x = xLen * (0.5 - realDis(rng));
    const G4double y = yLen * (0.5 - realDis(rng));

    return G4ThreeVector(x, y, zOffset);
}
