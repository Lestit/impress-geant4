#include <string>

#include <G4LogicalVolume.hh>
#include <G4Material.hh>
#include <G4PVPlacement.hh>
#include <G4Tubs.hh>
#include <G4VisAttributes.hh>

#include <ImpOnlyDetectorConstruction.hh>
#include <ImpGlobalConfigs.hh>
#include <ImpHafxChannel.hh>
#include <ImpMaterials.hh>

namespace {
    static const G4String BOUNDING_CYL = "single_det_bounding_cyl";
    static const G4String BOUNDING_LOG_VOL = "single_det_bounding_log_vol";
    static const G4String BOUNDING_PHY_VOL = "single_det_bounding_phy_vol";
}

ImpOnlyDetectorConstruction::ImpOnlyDetectorConstruction(const G4String& flareSize) :
    ImpVDetectorConstruction(),
    flareSize(flareSize)
{ }

ImpOnlyDetectorConstruction::~ImpOnlyDetectorConstruction()
{ }

G4VPhysicalVolume* ImpOnlyDetectorConstruction::Construct()
{
    constructEnclosing();
    constructPlaceDetector();

    return boundingPhysVol;
}

void ImpOnlyDetectorConstruction::ConstructSDandField()
{
    singleDetector->attachCrystalDetector();
    singleDetector->attachDumbSiDetector();
}

void ImpOnlyDetectorConstruction::constructEnclosing()
{
    static const G4double extraRoom = 0 * mm;
    static const G4double thickness = WORLD_DEPTH;
    const G4double boundingRad = ImpHafxChannel::radius() + extraRoom;

    boundingCylinder = new G4Tubs(
        BOUNDING_CYL, 0, boundingRad, thickness/2,
        0, 2 * pi);

    auto* vac = G4Material::GetMaterial(ImpMaterials::kVACUUM);
    boundingLogVol = new G4LogicalVolume(
        boundingCylinder, vac, BOUNDING_LOG_VOL);
    G4VisAttributes bva;
    bva.SetColor(0, 1, 1, 0.2);
    boundingLogVol->SetVisAttributes(bva);

    boundingPhysVol = new G4PVPlacement(
        nullptr, G4ThreeVector(), boundingLogVol, BOUNDING_PHY_VOL,
        nullptr, 0, 0, false);
}

void ImpOnlyDetectorConstruction::constructPlaceDetector()
{
    const G4double attThickness = ImpGlobalConf::ATTENUATOR_THICKNESSES.at(flareSize);
    const G4double totalThick = ImpHafxChannel::thicknessNoAttenuator() + attThickness;

    G4ThreeVector adjust(0, 0, 0);//-WORLD_DEPTH/2 + totalThick/2);
    singleDetector = new ImpHafxChannel(
        nullptr, adjust, boundingLogVol, "0", attThickness);
}
