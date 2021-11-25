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
#include "ImpSiSensitiveDetector.hh"

static G4OpticalSurface* teflonOpticalSurface();
static G4OpticalSurface* alOpticalSurface();
static G4OpticalSurface* beOpticalSurface();
static G4OpticalSurface* siOpticalSurface();
static G4OpticalSurface* qzOpticalSurface();
static G4OpticalSurface* cebr3OpticalSurface();


// i sure love constructors
ImpHafxChannel::ImpHafxChannel(
    G4RotationMatrix* rotMat, const G4ThreeVector& translate,
    G4LogicalVolume* motherLogVol, const G4String& channelId, const G4double attenuatorWindowThickness)
        : G4PVPlacement(rotMat, translate, tempLogVol(), CHANNEL_PFX + channelId, motherLogVol, false, 0),
    channelId(channelId),
    sensDetName(DUMB_SI_SENS_DET_PFX + channelId),
    attenuatorWindowThickness(attenuatorWindowThickness)
{
    quartzAnchorCenter = G4ThreeVector(
        0, 0, -thickness()/2 + QUARTZ_THICKNESS/2 + DUMB_SI_THICKNESS);
    cebr3AnchorCenter = quartzAnchorCenter + G4ThreeVector(
        0, 0, QUARTZ_THICKNESS/2 + CEBR3_THICKNESS/2);

    boundingCylinder = new G4Tubs(
        CHANNEL_CYL_PFX + channelId, 0, radius(), thickness()/2, 0 * deg, 360 * deg);
    auto* vac = G4Material::GetMaterial(ImpMaterials::kVACUUM);
    auto* logVol = new G4LogicalVolume(
        boundingCylinder, vac, WHOLE_CHANNEL_LOG_VOL_PFX + channelId);

    G4VisAttributes motherLogAttrs;
    motherLogAttrs.SetVisibility(true);
    motherLogAttrs.SetColor(1, 0, 0, 0.1);
    logVol->SetVisAttributes(motherLogAttrs);
    SetLogicalVolume(logVol);

    buildQuartz();
    buildCrystal();
    buildTeflonReflector();
    buildBeryllium();
    buildAlHousing();
    if (attenuatorWindowThickness > 0) buildAlAttenuator();
    buildDumbSi();
}

ImpHafxChannel::~ImpHafxChannel()
{ }

void ImpHafxChannel::buildCrystal()
{
    crystalCylinder = new G4Tubs(
        CRYSTAL_CYL_PFX + channelId, 0,
        CEBR3_DIAMETER / 2, CEBR3_THICKNESS/2, 0 * deg, 360 * deg);

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

    /* attachCeBr3OpticalSurface(); */
}

void ImpHafxChannel::buildTeflonReflector()
{
    static const G4double crystRad = CEBR3_DIAMETER / 2;
    static const G4double tefRad =
        (WHOLE_DIAMETER - 2*AL_HOUSING_THICKNESS) / 2;

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
    teflonAttrs.SetColor(1, 1, 1, 0.2);
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
    if (csd) { throw std::runtime_error("wot (IN ATTACH CRYSTAL DETECTOR)"); return; }

    csd = new ImpScintCrystalSensitiveDetector(CRYSTAL_SENS_DET_PFX + channelId, channelId);
    crystalSensDet.Put(csd);

    G4SDManager::GetSDMpointer()->AddNewDetector(csd);
    crystalLogVol->SetSensitiveDetector(csd);
}

void ImpHafxChannel::attachDumbSiDetector()
{
    auto* sisd = siSensDet.Get();
    if (sisd) { throw std::runtime_error("wot (IN ATTACH SILICON DETECTOR)"); return; }

    sisd = new ImpSiSensitiveDetector(sensDetName, channelId);
    siSensDet.Put(sisd);

    G4SDManager::GetSDMpointer()->AddNewDetector(sisd);
    dumbSiLogVol->SetSensitiveDetector(sisd);
}

void ImpHafxChannel::attachTeflonOpticalSurface()
{
    auto* surf = teflonOpticalSurface();
    tefSkin = new G4LogicalSkinSurface(
        TEFLON_SURF_PFX + channelId, tefLogVol, surf);
}

void ImpHafxChannel::buildAlHousing()
{
    static const G4double startRad = diameter()/2 - AL_HOUSING_THICKNESS;
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


    // account for attenuator window (and silicon at the back)
    G4ThreeVector alAnchorCenter(
        0, 0, -attenuatorWindowThickness/2 + DUMB_SI_THICKNESS/2);
    alPlacement = new G4PVPlacement(
        nullptr, alAnchorCenter, alLogVol, AL_PHY_PFX + channelId,
        GetLogicalVolume(), false, 0);

    attachAlOpticalSurface(alLogVol);
}

void ImpHafxChannel::attachAlOpticalSurface(G4LogicalVolume* curAlLogVol)
{
    // assume it's polished for now, but use the UNIFIED model,
    // taking the indices of refraction into account
    auto* alSurf = alOpticalSurface();
    alSkin = new G4LogicalSkinSurface(
        AL_SURF_PFX + channelId, curAlLogVol, alSurf);
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

    /* attachQuartzOpticalSurface(); */
}

void ImpHafxChannel::buildBeryllium()
{
    static const G4double beDiam = diameter() - 2 * AL_HOUSING_THICKNESS;
    beCylinder = new G4Tubs(
        BE_CYL_PFX + channelId, 0, beDiam / 2,
        BE_THICKNESS / 2, 0, 2*pi);

    auto* be = G4Material::GetMaterial(ImpMaterials::kBE);
    beLogVol = new G4LogicalVolume(
        beCylinder, be, BE_LOG_PFX + channelId);

    G4VisAttributes beAttrs;
    beAttrs.SetColor(1, 0, 1, 0.2);
    beAttrs.SetVisibility(true);
    beLogVol->SetVisAttributes(beAttrs);

    G4ThreeVector beAnchorAdjust = cebr3AnchorCenter + G4ThreeVector(
        0, 0, CEBR3_THICKNESS/2 + TEFLON_THICKNESS + BE_THICKNESS/2);
    bePlacement = new G4PVPlacement(
        nullptr, beAnchorAdjust, beLogVol, BE_PHY_PFX + channelId,
        GetLogicalVolume(), false, 0);

    attachBeOpticalSurface();
}

void ImpHafxChannel::attachBeOpticalSurface()
{
    auto* beSurf = beOpticalSurface();
    beSkin = new G4LogicalSkinSurface(
        BE_SUR_PFX + channelId, beLogVol, beSurf);
}

void ImpHafxChannel::buildAlAttenuator()
{
    attCylinder = new G4Tubs(
        ATT_CYL_PFX + channelId, 0, radius(),
        attenuatorWindowThickness / 2, 0, 2*pi);

    auto* al = G4Material::GetMaterial(ImpMaterials::kAL);
    attLogVol = new G4LogicalVolume(
        attCylinder, al, ATT_LOG_PFX + channelId);

    G4VisAttributes attAttrs;
    attAttrs.SetColor(0.7, 0.7, 0.7, 0.4);
    attAttrs.SetVisibility(true);
    attLogVol->SetVisAttributes(attAttrs);

    G4ThreeVector attTranslate(
        0, 0, thickness()/2 - attenuatorWindowThickness/2);
    attPlacement = new G4PVPlacement(
        nullptr, attTranslate, attLogVol, ATT_PHY_PFX + channelId,
        GetLogicalVolume(), false, 0);

    attachAlOpticalSurface(attLogVol);
}

void ImpHafxChannel::buildDumbSi()
{
    dumbSiCylinder = new G4Tubs(
        SI_CYL_PFX + channelId, 0, radius(),
        DUMB_SI_THICKNESS / 2, 0, 2*pi);

    auto* si = G4NistManager::Instance()
        ->FindOrBuildMaterial(ImpMaterials::kNIST_SI);

    G4VisAttributes siAttrs;
    siAttrs.SetColor(0, 0, 1, 0.3);
    siAttrs.SetVisibility(true);
    dumbSiLogVol = new G4LogicalVolume(
        dumbSiCylinder, si, SI_LOG_PFX + channelId);
    dumbSiLogVol->SetVisAttributes(siAttrs);

    auto siTranslate = quartzAnchorCenter + G4ThreeVector(
        0, 0, -QUARTZ_THICKNESS/2 - DUMB_SI_THICKNESS/2);
    dumbSiPlacement = new G4PVPlacement(
        nullptr, siTranslate, dumbSiLogVol, SI_PHY_PFX + channelId,
        GetLogicalVolume(), false, 0);

    attachDumbSiOpticalSurface();
}

void ImpHafxChannel::attachDumbSiOpticalSurface()
{
    auto* siSurf = siOpticalSurface();
    dumbSiSkin = new G4LogicalSkinSurface(
        SI_SUR_PFX + channelId, dumbSiLogVol, siSurf);
}

void ImpHafxChannel::attachQuartzOpticalSurface()
{
    auto* qs = qzOpticalSurface();
    qzSkin = new G4LogicalSkinSurface(
        QZ_SUR_PFX + channelId, qzLogVol, qs);
}

void ImpHafxChannel::attachCeBr3OpticalSurface()
{
    auto* s = cebr3OpticalSurface();
    new G4LogicalSkinSurface(
        CRYSTAL_SUR_PFX + channelId, crystalLogVol, s);
}

static G4OpticalSurface* siOpticalSurface()
{
    static G4OpticalSurface* ss = nullptr;
    if (ss) return ss;
    ss = new G4OpticalSurface(SI_SUR_PFX);
    ss->SetMaterialPropertiesTable(
        G4NistManager::Instance()
        ->FindOrBuildMaterial(ImpMaterials::kNIST_SI)
        ->GetMaterialPropertiesTable());
    ss->SetModel(unified);
    ss->SetFinish(polished);
    ss->SetType(dielectric_dielectric);
    return ss;
}

static G4OpticalSurface* teflonOpticalSurface()
{
    /* G4AutoLock l(&teflonMux); */
    static G4OpticalSurface* ts = nullptr;
    if (ts) return ts;
    ts = new G4OpticalSurface(TEFLON_SURF_PFX);
    ts->SetMaterialPropertiesTable(
        G4NistManager::Instance()->
        FindOrBuildMaterial(ImpMaterials::kNIST_TEFLON)->
        GetMaterialPropertiesTable());
    ts->SetType(dielectric_LUTDAVIS);
    ts->SetModel(DAVIS);
    // conveniently able to specify teflon as a surface :)
    ts->SetFinish(RoughTeflon_LUT);
    return ts;
}

static G4OpticalSurface* alOpticalSurface()
{
    /* G4AutoLock l(&alMux); */
    static G4OpticalSurface* as = nullptr;
    if (as) return as;
    as = new G4OpticalSurface(AL_SURF_PFX);
    as->SetMaterialPropertiesTable(
        G4Material::GetMaterial(ImpMaterials::kAL)->
        GetMaterialPropertiesTable());
    as->SetModel(unified);
    as->SetFinish(polished);
    as->SetType(dielectric_metal);
    return as;
}

static G4OpticalSurface* beOpticalSurface()
{
    static G4OpticalSurface* bs = nullptr;
    if (bs) return bs;
    bs = new G4OpticalSurface(BE_SUR_PFX);
    bs->SetMaterialPropertiesTable(
        G4Material::GetMaterial(ImpMaterials::kBE)->
        GetMaterialPropertiesTable());
    bs->SetType(dielectric_metal);
    bs->SetModel(unified);
    bs->SetFinish(ground);  // ground == rough
    return bs;
}

static G4OpticalSurface* qzOpticalSurface()
{
    static G4OpticalSurface* qs = nullptr;
    if (qs) return qs;
    qs = new G4OpticalSurface(QZ_SUR_PFX);
    qs->SetMaterialPropertiesTable(
        G4NistManager::Instance()->
        FindOrBuildMaterial(ImpMaterials::kNIST_SIO2)->
        GetMaterialPropertiesTable());
    qs->SetType(dielectric_dielectric);
    qs->SetModel(unified);
    qs->SetFinish(polished);
    return qs;
}

static G4OpticalSurface* cebr3OpticalSurface()
{
    static G4OpticalSurface* s = nullptr;
    if (s) return s;
    s = new G4OpticalSurface(CRYSTAL_SUR_PFX);
    s->SetMaterialPropertiesTable(
        G4Material::GetMaterial(ImpMaterials::kCEBR3)->
        GetMaterialPropertiesTable());
    s->SetModel(unified);
    s->SetType(dielectric_dielectric);
    s->SetFinish(polished);
    return s;
}
