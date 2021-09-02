#include "ImpHafxChannel.hh"

namespace {
    static const G4String CHANNEL_PFX = "hafx_channel";

    static const G4String CHANNEL_LOG_PFX = "hafx_logical";
    static const G4String CHANNEL_CYL_PFX = "hafx_bounding_cyl";
    static const G4String WHOLE_CHANNEL_LOG_VOL_PFX = "hafx_log_vol";

    static const G4String CRYSTAL_CYL_PFX = "hafx_crystal_cyl";
    static const G4String CRYSTAL_LOG_PFX = "hafx_crystal_log";
    static const G4String CRYSTAL_PHY_PFX = "hafx_crystal_phy";
}


ImpHafxChannel::ImpHafxChannel(G4RotationMatrix* rotMat, const G4ThreeVector& translate, G4LogicalVolume* motherLogVol, const G4String& channelId) : //(
    /* G4RotationMatrix* rotMat, const G4ThreeVector& translate, */
    /* G4LogicalVolume* motherLogVol, const G4String& channelId) : */
        G4PVPlacement(
                rotMat, translate,
                new G4LogicalVolume(
                    new G4Box("temp_box", 1, 1, 1), G4Material::GetMaterial(ImpMaterials::kVACUUM), "temp_lv"),
                CHANNEL_PFX + channelId, motherLogVol, false, 0),
        boundingCylinder(nullptr),
        channelId(channelId)
{
    boundingCylinder = new G4Tubs(CHANNEL_CYL_PFX + channelId, 0, radiusInMm(), thicknessInMm(), 0 * deg, 360 * deg);
    auto* vac = G4Material::GetMaterial(ImpMaterials::kVACUUM);
    wholeChannelLogVol = new G4LogicalVolume(boundingCylinder, vac, WHOLE_CHANNEL_LOG_VOL_PFX + channelId);
    wholeChannelLogVol->SetVisAttributes(G4VisAttributes::GetInvisible());

    buildCrystal();
    // buildTeflonReflector
    // buildAluminumBerylliumEnclosure
    // buildQuartz
    // buildSipm
    // ... etc
    SetLogicalVolume(wholeChannelLogVol);
}

ImpHafxChannel::~ImpHafxChannel()
{ }

void ImpHafxChannel::buildCrystal()
{
    crystalCylinder = new G4Tubs(
        CRYSTAL_CYL_PFX + channelId, 0, CEBR3_DIAMETER / 2, CEBR3_THICKNESS, 0 * deg, 360 * deg);

    auto* cebr3 = G4Material::GetMaterial(ImpMaterials::kCEBR3);
    crystalLogVol = new G4LogicalVolume(
        crystalCylinder, cebr3, CRYSTAL_LOG_PFX + channelId);

    G4VisAttributes crystAttrs;
    crystAttrs.SetVisibility(true);
    crystAttrs.SetColor(G4Color(0.35, 0.5, 0.92, 0.8));
    crystalLogVol->SetVisAttributes(crystAttrs);

    crystalPlacement = new G4PVPlacement(
        nullptr, G4ThreeVector(), crystalLogVol, CRYSTAL_PHY_PFX + channelId,
        wholeChannelLogVol, false, 0);
}
