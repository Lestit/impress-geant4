#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4String.hh"
#include "G4VisAttributes.hh"

#include "ImpDetectorPayload.hh"
#include "ImpMaterials.hh"
#include "ImpTempLogVol.hh"
#include "ImpGlobalConfigs.hh"

namespace {
    const G4String kPAYLOAD_ID = "imp_detector_payload";
    const G4String kBOUNDING_BOX = "imp_payload_bounding_box";
    const G4String kBOUNDING_LOG_VOL = "imp_payload_bounding_log_vol";

    const G4String PANEL_BOX = "imp_payload_front_panel_box";
    const G4String PANEL_PHY = "imp_payload_front_panel_phy";
    const G4String PANEL_LOG = "imp_payload_front_panel_log";
    const G4String PANEL_CRYST_HOLE_PFX = "imp_payload_front_panel_cryst_hole";
}

ImpDetectorPayload::ImpDetectorPayload(
    G4RotationMatrix* rotMat, const G4ThreeVector& translate, G4LogicalVolume* motherLogVol) :
    G4PVPlacement(rotMat, translate, tempLogVol(), kPAYLOAD_ID, motherLogVol, false, 0)
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
    static const G4double xyOffset = ImpHafxChannel::radius() + CRYSTAL_SEPARATION;
    std::vector<G4double> attenuatorThicknesses;
    for (const auto& p : ImpGlobalConf::ATTENUATOR_THICKNESSES) {
        attenuatorThicknesses.push_back(p.second);
    }

    for (size_t offsetIdx = 0; offsetIdx < NUM_CRYSTALS; ++offsetIdx) {
        G4int offy = offsetIdx / (NUM_CRYSTALS / 2);
        G4int offx = offsetIdx % (NUM_CRYSTALS / 2);
        G4ThreeVector pos(
            -xyOffset + 2*offx*xyOffset,
            -xyOffset + 2*offy*xyOffset,
            SIDE_LENGTH/2 - DETECTOR_OFFSET);

        crystalChannels[offsetIdx] = new ImpHafxChannel(
            nullptr, pos, GetLogicalVolume(),
            std::to_string(offsetIdx),
            attenuatorThicknesses[offsetIdx]);
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
    /* static const G4double holeOffset = ImpHafxChannel::radiusInMm() + CRYSTAL_SEPARATION; */
    /* static const G4double topPanelSide = 98 * mm; */
    /* static const G4double topPanelThick = 4.5 * mm; */
    
    /* // i am still dumbfounded as to why we need to specify half-lengths . . . */
    /* auto* topPanelBox = new G4Box( */
    /*     PANEL_BOX, topPanelSide / 2, topPanelSide / 2, topPanelThick / 2); */
    
    /* std::array<G4Tubs*, NUM_CRYSTALS> holes; */
    /* for (int i = 0; i < NUM_CRYSTALS; ++i) { */
    /*     holes[i] = new G4Tubs( */
    /*         P */
    /* } */
}
