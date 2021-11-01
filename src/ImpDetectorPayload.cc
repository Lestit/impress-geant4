#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4String.hh"
#include <G4SubtractionSolid.hh>
#include <G4Tubs.hh>
#include "G4VisAttributes.hh"

#include "ImpDetectorPayload.hh"
#include "ImpMaterials.hh"
#include "ImpTempLogVol.hh"
#include "ImpGlobalConfigs.hh"

namespace {
    const G4String kPAYLOAD_ID = "imp_detector_payload";
    const G4String kBOUNDING_BOX = "imp_payload_bounding_box";
    const G4String kBOUNDING_LOG_VOL = "imp_payload_bounding_log_vol";

    const G4String TPANEL_BOX = "imp_payload_front_panel_box";
    const G4String TPANEL_PHY = "imp_payload_front_panel_phy";
    const G4String TPANEL_LOG = "imp_payload_front_panel_log";
    const G4String TPANEL_SUB = "imp_payload_front_panel_subtr_sol";
    const G4String TPANEL_CRYST_HOLE_PFX = "imp_payload_front_panel_cryst_hole";
}

ImpDetectorPayload::ImpDetectorPayload(
    G4RotationMatrix* rotMat, const G4ThreeVector& translate, G4LogicalVolume* motherLogVol) :
    G4PVPlacement(rotMat, translate, tempLogVol(), kPAYLOAD_ID, motherLogVol, false, 0),
    topPanelSubSol(nullptr), topPanelPlacement(nullptr)
{
    const G4double half_sz = SIDE_LENGTH / 2;
    auto* boundingBox = new G4Box(
        kBOUNDING_BOX, half_sz, half_sz, half_sz);

    auto* vac = G4Material::GetMaterial(ImpMaterials::kVACUUM);
    auto* logVol = new G4LogicalVolume(
        boundingBox, vac, kBOUNDING_LOG_VOL);

    G4VisAttributes attrs;
    attrs.SetColor(1, 1, 1, 0.05);
    logVol->SetVisAttributes(attrs);
    SetLogicalVolume(logVol);

    buildPlaceChannels();
    buildPlaceTopPanel();
}

ImpDetectorPayload::~ImpDetectorPayload()
{ }

void ImpDetectorPayload::buildPlaceChannels()
{
    static const G4double xyOffset = CRYSTAL_HOLE_SPACING;
    std::vector<G4double> attenuatorThicknesses;
    for (const auto& p : ImpGlobalConf::ATTENUATOR_THICKNESSES) {
        if (p.first != "test") attenuatorThicknesses.push_back(p.second);
    }

    for (size_t offsetIdx = 0; offsetIdx < NUM_CRYSTALS; ++offsetIdx) {
        G4int offy = offsetIdx / (NUM_CRYSTALS / 2);
        G4int offx = offsetIdx % (NUM_CRYSTALS / 2);

        auto thick = attenuatorThicknesses[offsetIdx];
        G4double detectorOffset =
            ImpHafxChannel::thicknessNoAttenuator() / 2 +
            TOP_PANEL_THICK;

        G4ThreeVector pos(
            -xyOffset + 2*offx*xyOffset,
            -xyOffset + 2*offy*xyOffset,
            SIDE_LENGTH/2 - detectorOffset);

        crystalChannels[offsetIdx] = new ImpHafxChannel(
            nullptr, pos, GetLogicalVolume(),
            std::to_string(offsetIdx), thick);
    }
}

void ImpDetectorPayload::constructSensitiveDetectors()
{
    for (auto* channel : crystalChannels) {
        channel->attachCrystalDetector();
    }
}

void ImpDetectorPayload::buildPlaceTopPanel()
{
    /* // i am still dumbfounded as to why we need to specify half-lengths . . . */
    auto* topPanelBox = new G4Box(
        TPANEL_BOX, TOP_PANEL_SIDE / 2, TOP_PANEL_SIDE / 2, TOP_PANEL_THICK / 2);
    
    double shiftX[] = {
        -CRYSTAL_HOLE_SPACING, -CRYSTAL_HOLE_SPACING,
        CRYSTAL_HOLE_SPACING, CRYSTAL_HOLE_SPACING};
    double shiftY[] = {
        -CRYSTAL_HOLE_SPACING, CRYSTAL_HOLE_SPACING,
        -CRYSTAL_HOLE_SPACING, CRYSTAL_HOLE_SPACING};
    for (size_t i = 0; i < NUM_CRYSTALS; ++i) {
        auto* curHole = new G4Tubs(
            TPANEL_CRYST_HOLE_PFX + std::to_string(i), 0, CRYSTAL_ENTRANCE_DIAM / 2,
            TOP_PANEL_THICK * 0.51, 0, 2 * pi);
        G4VSolid* firstSubSol = (topPanelSubSol == nullptr)?
            static_cast<G4VSolid*>(topPanelBox)   :
            static_cast<G4VSolid*>(topPanelSubSol);

        G4ThreeVector shiftHole(shiftX[i], shiftY[i], 0);
        topPanelSubSol = new G4SubtractionSolid(
            TPANEL_SUB + std::to_string(i), firstSubSol, curHole, nullptr,
            shiftHole);
    }

    G4VisAttributes att;
    att.SetColor(1, 1, 1, 0.2);
    att.SetVisibility(true);

    auto* encAlloy = G4Material::GetMaterial(ImpMaterials::kHOUSING_ALLOY);
    auto* topPanelLogVol = new G4LogicalVolume(topPanelSubSol, encAlloy, TPANEL_LOG);
    topPanelLogVol->SetVisAttributes(att);

    G4double offset = SIDE_LENGTH/2 - TOP_PANEL_THICK/2;
    topPanelPlacement = new G4PVPlacement(
        nullptr, G4ThreeVector(0, 0, offset), topPanelLogVol, TPANEL_PHY, 
        GetLogicalVolume(), false, 0);
}
