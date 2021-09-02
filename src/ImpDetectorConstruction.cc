#include "ImpDetectorConstruction.hh"

ImpDetectorConstruction::ImpDetectorConstruction() :
    G4VUserDetectorConstruction(),
    crystalRotMat(std::make_shared<G4RotationMatrix>())
{
    /* crystalRotMat->rotateX(45 * deg); */
    /* crystalRotMat->rotateY(45 * deg); */
    // offload material construction to a separate free function
    ImpMaterials::makeMaterials();
}

ImpDetectorConstruction::~ImpDetectorConstruction()
{ /* all geometry objects are deleted by a VolumeStore object */ }


G4VPhysicalVolume* ImpDetectorConstruction::Construct()
{
    constructEnclosing();
    constructPlaceDetectors();

    return boundingBoxPhysVol;
}

void ImpDetectorConstruction::constructEnclosing()
{
    auto xy_sz = (IMPRESS_XY_MUL * CUBESAT_SIDE) + EXTRA_SPACE;
    auto z_sz = (IMPRESS_Z_MUL * CUBESAT_SIDE) + EXTRA_SPACE;
    boundingBox = new G4Box(kBOUNDING_BOX, xy_sz, xy_sz, z_sz);
    
    auto* vacMat = G4Material::GetMaterial(ImpMaterials::kVACUUM);
    if (!vacMat) throw std::runtime_error("vacuum not defined");
    boundingBoxLogVol = new G4LogicalVolume(
        boundingBox, vacMat, kBOUNDING_LOG_VOL);

    G4VisAttributes boundingBoxAttrs;
    boundingBoxAttrs.SetColor(0.8, 0.8, 0.8, 0.1);
    boundingBoxAttrs.SetVisibility(true);
    boundingBoxLogVol->SetVisAttributes(boundingBoxAttrs);

    G4ThreeVector noTranslation;
    boundingBoxPhysVol = new G4PVPlacement(
        nullptr, noTranslation, boundingBoxLogVol, kBOUNDING_PHY_VOL,
        nullptr, 0, 0, false);
}

void ImpDetectorConstruction::constructPlaceDetectors()
{
    G4double xy_offst = 4 * cm;
    G4ThreeVector pos;
    for (G4int offx = 0; offx < 2; ++offx) {
        for (G4int offy = 0; offy < 2; ++offy) {
            pos = G4ThreeVector(
                    -xy_offst + 2*offx*xy_offst,
                    -xy_offst + 2*offy*xy_offst,
                    0);
            crystalChannels[2*offx + offy] = new ImpHafxChannel(crystalRotMat.get(), pos, boundingBoxLogVol, "test");
        }
    }
}

void ImpDetectorConstruction::ConstructSDandField()
// deal with this later
{ return; }
