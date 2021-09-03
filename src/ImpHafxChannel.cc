#include "ImpHafxChannel.hh"

namespace {
    static const G4String CHANNEL_PFX = "hafx_channel";

    static const G4String CHANNEL_LOG_PFX = "hafx_logical";
    static const G4String CHANNEL_CYL_PFX = "hafx_bounding_cyl";
    static const G4String WHOLE_CHANNEL_LOG_VOL_PFX = "hafx_log_vol";

    static const G4String CRYSTAL_CYL_PFX = "hafx_crystal_cyl";
    static const G4String CRYSTAL_LOG_PFX = "hafx_crystal_log";
    static const G4String CRYSTAL_PHY_PFX = "hafx_crystal_phy";

    static const G4String TEFLON_RING = "hafx_teflon_ring";
    static const G4String TEFLON_CAP = "hafx_teflon_cap";
    static const G4String TEFLON_SHAPE = "hafx_teflon_union";
    static const G4String TEFLON_RING_LOG_PFX = "hafx_teflon_ring_log";
    static const G4String TEFLON_CAP_LOG_PFX = "hafx_teflon_cap_log";
    static const G4String TEFLON_RING_PHY_PFX = "hafx_teflon_ring_phy";
    static const G4String TEFLON_CAP_PHY_PFX = "hafx_teflon_cap_phy";
}


ImpHafxChannel::ImpHafxChannel(G4RotationMatrix* rotMat, const G4ThreeVector& translate, G4LogicalVolume* motherLogVol, const G4String& channelId) : //(
        G4PVPlacement(
                rotMat, translate,
                tempLogVol(),
                CHANNEL_PFX + channelId, motherLogVol, false, 0),
        adjustPlacements(0, 0, (CEBR3_THICKNESS - thicknessInMm())/2),
        channelId(channelId)
{
    boundingCylinder = new G4Tubs(CHANNEL_CYL_PFX + channelId, 0, radiusInMm(), thicknessInMm()/2, 0 * deg, 360 * deg);
    auto* vac = G4Material::GetMaterial(ImpMaterials::kVACUUM);
    auto* logVol = new G4LogicalVolume(boundingCylinder, vac, WHOLE_CHANNEL_LOG_VOL_PFX + channelId);
    G4VisAttributes motherLogAttrs;
    motherLogAttrs.SetVisibility(true);
    motherLogAttrs.SetColor(1, 0, 0, 0.3);
    logVol->SetVisAttributes(motherLogAttrs);
    SetLogicalVolume(logVol);

    buildCrystal();
    buildTeflonReflector();
    // buildAluminumBerylliumEnclosure
    // buildQuartz
    // buildSipm
    // ... etc
}

ImpHafxChannel::~ImpHafxChannel()
{ }

void ImpHafxChannel::buildCrystal()
{
    crystalCylinder = new G4Tubs(
        CRYSTAL_CYL_PFX + channelId, 0, CEBR3_DIAMETER / 2, CEBR3_THICKNESS/2, 0 * deg, 360 * deg);

    auto* cebr3 = G4Material::GetMaterial(ImpMaterials::kCEBR3);
    crystalLogVol = new G4LogicalVolume(
        crystalCylinder, cebr3, CRYSTAL_LOG_PFX + channelId);

    G4VisAttributes crystAttrs;
    crystAttrs.SetVisibility(true);
    crystAttrs.SetColor(G4Color(0.35, 0.5, 0.92, 0.8));
    crystalLogVol->SetVisAttributes(crystAttrs);

    crystalPlacement = new G4PVPlacement(
        nullptr, adjustPlacements, crystalLogVol, CRYSTAL_PHY_PFX + channelId,
        GetLogicalVolume(), false, 0);
}

void ImpHafxChannel::buildTeflonReflector()
{
    static const G4double crystRad = CEBR3_DIAMETER / 2;
    static const G4double tefRad = crystRad + TEFLON_THICKNESS;

    tefRing = new G4Tubs(
        TEFLON_RING + channelId, crystRad, tefRad, CEBR3_THICKNESS/2, 0, 2*pi);
    tefCap = new G4Tubs(
        TEFLON_CAP + channelId, 0, tefRad, TEFLON_THICKNESS/2, 0, 2*pi);

    auto* ptfe = G4NistManager::Instance()->FindOrBuildMaterial(ImpMaterials::kNIST_TEFLON);
    tefRingLogVol = new G4LogicalVolume(
        tefRing, ptfe, TEFLON_RING_LOG_PFX + channelId);
    tefCapLogVol = new G4LogicalVolume(
        tefCap, ptfe, TEFLON_CAP_LOG_PFX + channelId);
    
    G4VisAttributes teflonAttrs;
    teflonAttrs.SetColor(1, 1, 1, 0.5);
    teflonAttrs.SetVisibility(true);
    tefCapLogVol->SetVisAttributes(teflonAttrs);
    tefRingLogVol->SetVisAttributes(teflonAttrs);

    tefRingPlacement = new G4PVPlacement(
        nullptr, adjustPlacements, tefRingLogVol, TEFLON_RING_PHY_PFX + channelId,
        GetLogicalVolume(), false, 0);

    G4ThreeVector displace(0, 0, (CEBR3_THICKNESS + TEFLON_THICKNESS) / 2);
    displace += adjustPlacements;
    tefCapPlacement = new G4PVPlacement(
        nullptr, displace, tefCapLogVol, TEFLON_CAP_PHY_PFX + channelId,
        GetLogicalVolume(), false, 0);
}
