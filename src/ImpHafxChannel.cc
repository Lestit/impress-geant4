#include <G4AutoLock.hh>
#include <G4Box.hh>
#include <G4Color.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4LogicalVolume.hh>
#include <G4NistManager.hh>
#include <G4OpticalSurface.hh>
#include <G4RotationMatrix.hh>
#include <G4SDmanager.hh>
#include <G4Tubs.hh>
#include <G4SubtractionSolid.hh>
#include <G4UnionSolid.hh>
#include <G4VisAttributes.hh>

#include <ImpHafxChannel.hh>
#include <ImpMaterials.hh>
#include <ImpTempLogVol.hh>
#include <ImpScintCrystalSensitiveDetector.hh>
#include <ImpSiSensitiveDetector.hh>

static G4OpticalSurface* teflonOpticalSurface();
static G4OpticalSurface* alOpticalSurface();
static G4OpticalSurface* beOpticalSurface();
static G4OpticalSurface* siOpticalSurface();
static G4OpticalSurface* qzOpticalSurface();
static G4OpticalSurface* cebr3OpticalSurface();
static G4OpticalSurface* paintSurface();


// i sure love constructors
ImpHafxChannel::ImpHafxChannel(
    G4RotationMatrix* rotMat, const G4ThreeVector& translate,
    G4LogicalVolume* motherLogVol, const G4String& channelId,
    const G4double attenuatorWindowThickness)
        : G4PVPlacement(rotMat, translate, tempLogVol(), CHANNEL_PFX + channelId, motherLogVol, false, 0),
    channelId(channelId),
    sensDetName(SI_SENS_DET_PFX + channelId),
    attenuatorWindowThickness(attenuatorWindowThickness)
{
    quartzAnchorCenter = G4ThreeVector(
        0, 0, -thickness()/2 + QUARTZ_THICKNESS/2 + LIGHT_GUIDE_THICKNESS +SI_THICKNESS);
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
    buildSi();
    buildLightGuide();
    buildPaint();
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

    attachTeflonOpticalSurface(tefLogVol);
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

void ImpHafxChannel::attachSiDetector()
{
    for (const auto* slv : siLogVols)
        if (!slv) return;

    auto* sisd = siSensDet.Get();
    if (sisd) { throw std::runtime_error("wot (IN ATTACH SILICON DETECTOR)"); return; }

    sisd = new ImpSiSensitiveDetector(sensDetName, channelId);
    siSensDet.Put(sisd);

    G4SDManager::GetSDMpointer()->AddNewDetector(sisd);
    for (auto* slv : siLogVols)
        slv->SetSensitiveDetector(sisd);
}

void ImpHafxChannel::attachAllDetectors()
{
    attachCrystalDetector();
    attachSiDetector();
}

void ImpHafxChannel::attachTeflonOpticalSurface(G4LogicalVolume* tlv)
{
    auto* surf = teflonOpticalSurface();
    tefSkin = new G4LogicalSkinSurface(
        TEFLON_SURF_PFX + tlv->GetName() + channelId, tlv, surf);
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
        0, 0, -attenuatorWindowThickness/2 + SI_THICKNESS/2 + LIGHT_GUIDE_THICKNESS/2);
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

void ImpHafxChannel::buildLightGuide()
{
    const auto hxy = LIGHT_GUIDE_SIDE_LENGTH / 2, hz = LIGHT_GUIDE_THICKNESS / 2;
    lightGuideBox = new G4Box(LG_BOX_PFX + channelId, hxy, hxy, hz);

    // make quartz for now...
    auto* lgMaterial = G4NistManager::Instance()
        ->FindOrBuildMaterial(ImpMaterials::kNIST_SIO2);

    G4VisAttributes lgAtt;
    lgAtt.SetColor(0.686, 0.521, 1, 0.3);
    lgAtt.SetVisibility(true);
    lightGuideLogVol = new G4LogicalVolume(
        lightGuideBox, lgMaterial, LG_LOG_PFX + channelId);
    lightGuideLogVol->SetVisAttributes(lgAtt);

    const auto wrapThick = TEFLON_THICKNESS * mm;
    lightGuideWrapToBeCutoutBox = new G4Box(
        LG_WRAP_BOX_PFX + channelId, hxy + wrapThick/2, hxy + wrapThick/2, hz);
    // just to cut the end caps off nicely
    auto* sliceOut = new G4Box("", hxy, hxy, hz + 1 * mm);
    auto* slicedRim = new G4SubtractionSolid(
        LG_WRAP_SOL_PFX + "_rim_sliced" + channelId, lightGuideWrapToBeCutoutBox, sliceOut);

    // "sihxy" is kind of a funny name haha
    const auto sihxy = SI_SIDE_LENGTH / 2, sihz = SI_THICKNESS / 2;
    auto* sliceOutSi = new G4Box("", sihxy, sihxy, sihz + 1 * mm);
    auto* capToSlice = new G4Box("", hxy, hxy, wrapThick / 2);
    auto* slicedCap = new G4SubtractionSolid(
        LG_WRAP_SOL_PFX + "_cap_sliced" + channelId, capToSlice, sliceOutSi);

    G4ThreeVector displaceWrapPieces(0, 0, -hz - wrapThick/2);
    lightGuideWrapSolid = new G4UnionSolid(
        LG_WRAP_SOL_PFX + channelId, slicedRim, slicedCap, nullptr, displaceWrapPieces);

    auto* ptfe = G4NistManager::Instance()
        ->FindOrBuildMaterial(ImpMaterials::kNIST_TEFLON);
    lightGuideWrapLogVol = new G4LogicalVolume(
        lightGuideWrapSolid, ptfe, LG_WRAP_LOG_PFX + channelId);
    attachTeflonOpticalSurface(lightGuideWrapLogVol);

    G4VisAttributes wrAtt;
    wrAtt.SetColor(1, 0, 0, 0.1);
    wrAtt.SetVisibility(true);
    lightGuideWrapLogVol->SetVisAttributes(wrAtt);

    const auto lgTranslate = quartzAnchorCenter + G4ThreeVector(
        0, 0, -QUARTZ_THICKNESS/2 - LIGHT_GUIDE_THICKNESS/2);
    lightGuidePlacement = new G4PVPlacement(
        nullptr, lgTranslate, lightGuideLogVol, LG_PHY_PFX + channelId,
        GetLogicalVolume(), false, 0);
    lightGuideWrapPlacement = new G4PVPlacement(
        nullptr, lgTranslate, lightGuideWrapLogVol, LG_WRAP_PHY_PFX + channelId,
        GetLogicalVolume(), false, 0);
}

void ImpHafxChannel::buildPaint()
{
    const auto radius = CEBR3_DIAMETER / 2;
    const auto thick = 1 * mm;
    auto* baseCyl = new G4Tubs(
        PAINT_CYL_PFX + channelId, 0, radius, thick/2,
        0, 2*pi);
    
    auto* paintSubSol = new G4SubtractionSolid(
        PAINT_SOL_PFX + channelId, baseCyl, lightGuideWrapToBeCutoutBox);
    auto* tempMat = G4Material::GetMaterial(ImpMaterials::kVACUUM);
    auto* paintLogVol = new G4LogicalVolume(
        paintSubSol, tempMat, PAINT_LOG_PFX + channelId);
    G4VisAttributes pAtt;
    pAtt.SetColor(0, 0, 0, 0.1);
    pAtt.SetVisibility(true);
    paintLogVol->SetVisAttributes(pAtt);

    auto paintTranslate = quartzAnchorCenter + G4ThreeVector(
        0, 0, -QUARTZ_THICKNESS/2 - thick/2);
    auto* paintPlacement = new G4PVPlacement(
        nullptr, paintTranslate, paintLogVol, PAINT_PHY_PFX + channelId,
        GetLogicalVolume(), false, 0);

    attachPaintBoundarySurface(paintPlacement, paintLogVol);
}

void ImpHafxChannel::buildSi()
{
    const auto halfxy = BROADCOM_FULL_LENGTH / 2, halfz = SI_THICKNESS / 2;
    const auto deltaxy = BROADCOM_FULL_LENGTH + SI_SPACING;
    /* siBox = new G4Box(SI_BOX_PFX + channelId, half_xy, half_xy, half_z); */
    auto* si = G4NistManager::Instance()
        ->FindOrBuildMaterial(ImpMaterials::kNIST_SI);
    auto baseTranslate = quartzAnchorCenter + G4ThreeVector(
        0, 0, -QUARTZ_THICKNESS/2 - LIGHT_GUIDE_THICKNESS - SI_THICKNESS/2);
    G4VisAttributes siAttrs;
    siAttrs.SetColor(0, 0, 1, 0.3);
    siAttrs.SetVisibility(true);

    for (size_t i = 0; i < ImpHafxChannel::NUM_SIPMS; ++i) {
        const auto xtlate = (i%4 - 1.5) * deltaxy;
        const auto ytlate = (i/4 - 1.5) * deltaxy;
        const auto idxStr = std::to_string(i);

        auto* siBox = new G4Box(
            SI_BOX_PFX + idxStr + channelId, halfxy, halfxy, halfz);
        auto* siLogVol = new G4LogicalVolume(
            siBox, si, SI_LOG_PFX + idxStr + channelId);
        siLogVol->SetVisAttributes(siAttrs);

        auto siTranslate = baseTranslate + G4ThreeVector(xtlate, ytlate, 0);
        (void) new G4PVPlacement(
            nullptr, siTranslate, siLogVol, SI_PHY_PFX + channelId,
            GetLogicalVolume(), false, 0);
        siBoxes[i] = siBox;
        siLogVols[i] = siLogVol;
    }

    attachSiOpticalSurface();
}

void ImpHafxChannel::attachSiOpticalSurface()
{
    auto* siSurf = siOpticalSurface();
    for (size_t i = 0; i < siLogVols.size(); ++i) {
        (void) new G4LogicalSkinSurface(
            SI_SUR_PFX + std::to_string(i) + channelId, siLogVols[i], siSurf);
    }
}

void ImpHafxChannel::attachQuartzOpticalSurface()
{
    auto* qs = qzOpticalSurface();
    qzSkin = new G4LogicalSkinSurface(
        QZ_SUR_PFX + channelId, qzLogVol, qs);
}

void ImpHafxChannel::attachCeBr3OpticalSurface()
{
    auto* sur = cebr3OpticalSurface();
    new G4LogicalSkinSurface(
        CRYSTAL_SUR_PFX + channelId, crystalLogVol, sur);
}

void ImpHafxChannel::attachPaintBoundarySurface(G4VPhysicalVolume* ppv, G4LogicalVolume* plv)
{
    auto* paintSur = paintSurface();
    new G4LogicalSkinSurface(
        PAINT_SUR_PFX + channelId, plv, paintSur);
    new G4LogicalBorderSurface(
        PAINT_SUR_PFX + channelId, ppv, crystalPlacement, paintSur);
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
    static G4OpticalSurface* sur = nullptr;
    if (sur) return sur;
    sur = new G4OpticalSurface(CRYSTAL_SUR_PFX);
    sur->SetMaterialPropertiesTable(
        G4Material::GetMaterial(ImpMaterials::kCEBR3)->
        GetMaterialPropertiesTable());
    sur->SetModel(unified);
    sur->SetType(dielectric_dielectric);
    sur->SetFinish(polished);
    return sur;
}

static G4OpticalSurface* paintSurface()
{
    static G4OpticalSurface* sur = nullptr;
    if (sur) return sur;
    sur = new G4OpticalSurface(PAINT_SUR_PFX);
    sur->SetModel(LUT);
    sur->SetType(dielectric_LUT);
    // from https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/TrackingAndPhysics/physicsProcess.html#codeblock62
    // chemically etched surface, with tio paint
    sur->SetFinish(groundtioair);
    return sur;
}
