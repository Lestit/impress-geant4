#include "G4AutoLock.hh"
#include "G4Box.hh"
#include "G4Color.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4OpticalSurface.hh"
#include "G4RotationMatrix.hh"
#include "G4SDmanager.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4VisAttributes.hh"

#include "ImpHafxChannel.hh"
#include "ImpMaterials.hh"
#include "ImpTempLogVol.hh"
#include "ImpScintCrystalSensitiveDetector.hh"

namespace {
    G4Mutex mux = G4MUTEX_INITIALIZER;
    G4OpticalSurface* tefSurf_ = nullptr;

    static const G4String CHANNEL_PFX = "hafx_channel";

    static const G4String CHANNEL_LOG_PFX = "hafx_logical";
    static const G4String CHANNEL_CYL_PFX = "hafx_bounding_cyl";
    static const G4String WHOLE_CHANNEL_LOG_VOL_PFX = "hafx_log_vol";

    static const G4String CRYSTAL_CYL_PFX = "hafx_crystal_cyl";
    static const G4String CRYSTAL_LOG_PFX = "hafx_crystal_log";
    static const G4String CRYSTAL_PHY_PFX = "hafx_crystal_phy";

    static const G4String TEFLON_RING = "hafx_teflon_ring";
    static const G4String TEFLON_CAP = "hafx_teflon_cap";
    static const G4String TEFLON_SHAPE_PFX = "hafx_teflon_union";
    static const G4String TEFLON_LOG_PFX = "hafx_teflon_log";
    static const G4String TEFLON_PHY_PFX = "hafx_teflon_phy";
    static const G4String TEFLON_SURF_PFX = "hafx_teflon_surf";
    static const G4String TEFLON_SKIN_LOG_PFX = "hafx_teflon_skin_log";

    static const G4String SENSITIVE_DET_PFX = "hafx_crystal_sd";
}


// i sure love constructors
ImpHafxChannel::ImpHafxChannel(
    G4RotationMatrix* rotMat, const G4ThreeVector& translate,
    G4LogicalVolume* motherLogVol, const G4String& channelId)
    : G4PVPlacement(rotMat, translate,
        tempLogVol(), CHANNEL_PFX + channelId, motherLogVol, false, 0),
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

    auto* tefRing = new G4Tubs(
        TEFLON_RING + channelId, crystRad, tefRad, CEBR3_THICKNESS/2, 0, 2*pi);
    auto* tefCap = new G4Tubs(
        TEFLON_CAP + channelId, 0, tefRad, TEFLON_THICKNESS/2, 0, 2*pi);

    G4ThreeVector displace(0, 0, (CEBR3_THICKNESS + TEFLON_THICKNESS) / 2);
    tefSolid = new G4UnionSolid(
        TEFLON_SHAPE_PFX + channelId, tefRing, tefCap, nullptr, displace);

    auto* ptfe = G4NistManager::Instance()->FindOrBuildMaterial(ImpMaterials::kNIST_TEFLON);
    tefLogVol = new G4LogicalVolume(
        tefSolid, ptfe, TEFLON_LOG_PFX + channelId);
    
    G4VisAttributes teflonAttrs;
    teflonAttrs.SetColor(1, 1, 1, 0.5);
    teflonAttrs.SetVisibility(true);
    tefLogVol->SetVisAttributes(teflonAttrs);

    tefPlacement = new G4PVPlacement(
        nullptr, adjustPlacements, tefLogVol, TEFLON_PHY_PFX + channelId,
        GetLogicalVolume(), false, 0);

    attachTeflonOpticalSurface();
}

void ImpHafxChannel::attachCrystalDetector()
{
    ImpScintCrystalSensitiveDetector* csd = crystalSensDet.Get();
    if (csd) { return; }

    csd = new ImpScintCrystalSensitiveDetector(SENSITIVE_DET_PFX + channelId, channelId);
    crystalSensDet.Put(csd);

    G4SDManager::GetSDMpointer()->AddNewDetector(csd);
    crystalLogVol->SetSensitiveDetector(csd);
}

G4OpticalSurface* ImpHafxChannel::tefSurf()
{
    //G4AutoLock l(&mux);
    if (tefSurf_) return tefSurf_;
    tefSurf_ = new G4OpticalSurface(TEFLON_SURF_PFX);
    tefSurf_->SetType(dielectric_LUTDAVIS);
    tefSurf_->SetModel(DAVIS);
    tefSurf_->SetFinish(RoughTeflon_LUT);
    return tefSurf_;
}

void ImpHafxChannel::attachTeflonOpticalSurface()
{
    G4AutoLock l(&mux);
    auto* surf = tefSurf();
    // tefSurf = new G4OpticalSurface(TEFLON_SURF_PFX + channelId);
    // conveniently able to specify teflon as a surface :)
    tefSkin = new G4LogicalSkinSurface(
        TEFLON_SURF_PFX + channelId, tefLogVol, surf);
}
