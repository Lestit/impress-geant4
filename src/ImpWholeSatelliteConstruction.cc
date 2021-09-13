#include "ImpWholeSatelliteConstruction.hh"

ImpWholeSatelliteConstruction::ImpWholeSatelliteConstruction() :
    ImpVDetectorConstruction()
{ }

ImpWholeSatelliteConstruction::~ImpWholeSatelliteConstruction()
{ /* all geometry objects are deleted by a VolumeStore object */ }


G4VPhysicalVolume* ImpWholeSatelliteConstruction::Construct()
{
    constructEnclosing();

    static const G4ThreeVector payloadPosition(0, 0, 1 * CUBESAT_SIDE);
    payload = new ImpDetectorPayload(nullptr, payloadPosition, boundingBoxLogVol);

    return boundingBoxPhysVol;
}

void ImpWholeSatelliteConstruction::constructEnclosing()
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

void ImpWholeSatelliteConstruction::ConstructSDandField()
// deal with this later
{
    payload->constructSensitiveDetectors();
}
