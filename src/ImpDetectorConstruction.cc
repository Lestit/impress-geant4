#include "ImpDetectorConstruction.hh"

ImpDetectorConstruction::ImpDetectorConstruction() :
    G4VUserDetectorConstruction()
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

    static const G4ThreeVector payloadPosition(0, 0, 1 * CUBESAT_SIDE);
    G4cout << "making payload\n";
    G4cout.flush();
    new ImpDetectorPayload(nullptr, payloadPosition, boundingBoxLogVol);

    return boundingBoxPhysVol;
}

void ImpDetectorConstruction::constructEnclosing()
{
    const auto xy_half_sz = (IMPRESS_UNITS_WIDE * CUBESAT_SIDE + EXTRA_SPACE) / 2;
    const auto z_half_sz = (IMPRESS_UNITS_LONG * CUBESAT_SIDE + EXTRA_SPACE) / 2;
    boundingBox = new G4Box(kBOUNDING_BOX, xy_half_sz, xy_half_sz, z_half_sz);
    
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

void ImpDetectorConstruction::ConstructSDandField()
// deal with this later
{ return; }
