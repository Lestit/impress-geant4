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
    G4Mutex teflonMux = G4MUTEX_INITIALIZER;
    G4Mutex alMux = G4MUTEX_INITIALIZER;

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

    static const G4String AL_CYL_PFX = "hafx_al_cyl";
    static const G4String AL_LOG_PFX = "hafx_al_log";
    static const G4String AL_PHY_PFX = "hafx_al_phy";
    static const G4String AL_SURF_PFX = "hafx_al_surf";

    static const G4String QZ_CYL_PFX = "hafx_qz_cyl";
    static const G4String QZ_LOG_PFX = "hafx_qz_log";
    static const G4String QZ_PHY_PFX = "hafx_qz_phy";

    static const G4String SENSITIVE_DET_PFX = "hafx_crystal_sd";
}

// i sure love constructors
ImpHafxChannel::ImpHafxChannel(
    G4RotationMatrix* rotMat, const G4ThreeVector& translate,
    G4LogicalVolume* motherLogVol, const G4String& channelId)
        : G4PVPlacement(rotMat, translate, tempLogVol(), CHANNEL_PFX + channelId, motherLogVol, false, 0),
    quartzAnchorCenter(0, 0, -(thicknessInMm() - QUARTZ_THICKNESS)/2),
    channelId(channelId)
{
    cebr3AnchorCenter = quartzAnchorCenter + G4ThreeVector(0, 0, (QUARTZ_THICKNESS + CEBR3_THICKNESS)/2);

    boundingCylinder = new G4Tubs(CHANNEL_CYL_PFX + channelId, 0, radiusInMm(), thicknessInMm()/2, 0 * deg, 360 * deg);
    auto* vac = G4Material::GetMaterial(ImpMaterials::kVACUUM);
    auto* logVol = new G4LogicalVolume(boundingCylinder, vac, WHOLE_CHANNEL_LOG_VOL_PFX + channelId);

    G4VisAttributes motherLogAttrs;
    motherLogAttrs.SetVisibility(true);
    motherLogAttrs.SetColor(1, 0, 0, 0.1);
    logVol->SetVisAttributes(motherLogAttrs);
    SetLogicalVolume(logVol);

    buildCrystal();
    buildTeflonReflector();
    buildAlHousing();
    buildQuartz();
    // buildSipm
    // ... etc
}

ImpHafxChannel::~ImpHafxChannel()
{ }

G4OpticalSurface* teflonOpticalSurface()
{
    G4AutoLock l(&teflonMux);
    static G4OpticalSurface* ts;
    if (ts) return ts;
    ts = new G4OpticalSurface(TEFLON_SURF_PFX);
    ts->SetType(dielectric_LUTDAVIS);
    ts->SetModel(DAVIS);
    // conveniently able to specify teflon as a surface :)
    ts->SetFinish(RoughTeflon_LUT);
    ts->SetMaterialPropertiesTable(
        G4NistManager::Instance()->
        FindOrBuildMaterial(ImpMaterials::kNIST_TEFLON)->
        GetMaterialPropertiesTable());
    return ts;
}

G4OpticalSurface* alOpticalSurface()
{
    G4AutoLock l(&alMux);
    static G4OpticalSurface* as;
    if (as) return as;
    as = new G4OpticalSurface(AL_SURF_PFX);
    as->SetModel(unified);
    as->SetFinish(polished);
    as->SetType(dielectric_metal);
    as->SetMaterialPropertiesTable(
        G4NistManager::Instance()->
        FindOrBuildMaterial(ImpMaterials::kAL)->
        GetMaterialPropertiesTable());
    return as;
}

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
        nullptr, cebr3AnchorCenter, crystalLogVol, CRYSTAL_PHY_PFX + channelId,
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
        nullptr, cebr3AnchorCenter, tefLogVol, TEFLON_PHY_PFX + channelId,
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

void ImpHafxChannel::attachTeflonOpticalSurface()
{
    auto* surf = teflonOpticalSurface();
    tefSkin = new G4LogicalSkinSurface(
        TEFLON_SURF_PFX + channelId, tefLogVol, surf);
}

void ImpHafxChannel::buildAlHousing()
{
    static const G4double startRad = CEBR3_DIAMETER/2 + TEFLON_THICKNESS;
    static const G4double endRad = startRad + AL_HOUSING_THICKNESS;

    alCylinder = new G4Tubs(
        AL_CYL_PFX + channelId,
        startRad, endRad,
        AL_HOUSING_DEPTH/2, 0, 2*pi);

    auto* al = G4Material::GetMaterial(ImpMaterials::kAL);
    alLogVol = new G4LogicalVolume(
        alCylinder, al, AL_LOG_PFX + channelId);
    G4VisAttributes alAttrs;
    alAttrs.SetColor(0, 1, 0, 0.1);
    alAttrs.SetVisibility(true);
    alLogVol->SetVisAttributes(alAttrs);


    // aluminum fills the whole thing
    alPlacement = new G4PVPlacement(
        nullptr, G4ThreeVector(), alLogVol, AL_PHY_PFX + channelId,
        GetLogicalVolume(), false, 0);

    attachAlOpticalSurface();
}

void ImpHafxChannel::attachAlOpticalSurface()
{
    // assume it's polished for now, but use the UNIFIED model,
    // taking the indices of refraction into account
    auto* alSurf = alOpticalSurface();
    alSkin = new G4LogicalSkinSurface(
        AL_SURF_PFX + channelId, alLogVol, alSurf);
}

void ImpHafxChannel::buildQuartz()
{
    qzCylinder = new G4Tubs(
        QZ_CYL_PFX + channelId, 0, QUARTZ_DIAMETER / 2,
        QUARTZ_THICKNESS/2, 0, 2*pi);

    auto* qz = G4NistManager::Instance()->FindOrBuildMaterial(ImpMaterials::kNIST_SIO2);
    qzLogVol = new G4LogicalVolume(
        qzCylinder, qz, QZ_LOG_PFX + channelId);

    G4VisAttributes qzAttrs;
    qzAttrs.SetColor(1, 0.6, 0, 0.1);
    qzAttrs.SetVisibility(true);
    qzLogVol->SetVisAttributes(qzAttrs);

    qzPlacement = new G4PVPlacement(
        nullptr, quartzAnchorCenter, qzLogVol, QZ_PHY_PFX + channelId,
        GetLogicalVolume(), false, 0);
}
