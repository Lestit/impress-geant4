#include "ImpDetectorPayload.hh"

namespace {
    const G4String kPAYLOAD_ID = "imp_detector_payload";
    const G4String kBOUNDING_BOX = "imp_payload_bounding_box";
    const G4String kBOUNDING_LOG_VOL = "imp_payload_bounding_log_vol";
    const G4String CRYSTAL_PREFIX = "imp_payload_channel";
}

ImpDetectorPayload::ImpDetectorPayload(
    G4RotationMatrix* rotMat, const G4ThreeVector& translate, G4LogicalVolume* motherLogVol) :
    G4PVPlacement(rotMat, translate, tempLogVol(), kPAYLOAD_ID, motherLogVol, false, 0)
{
    G4cout << "inside payload\n";
    G4cout.flush();

    const G4double half_sz = SIDE_LENGTH / 2;
    boundingBox = new G4Box(
        kBOUNDING_BOX, half_sz, half_sz, half_sz);

    auto* vac = G4Material::GetMaterial(ImpMaterials::kVACUUM);
    auto* logVol = new G4LogicalVolume(
        boundingBox, vac, kBOUNDING_LOG_VOL);

    G4VisAttributes attrs;
    attrs.SetColor(1, 1, 1, 0.05);
    logVol->SetVisAttributes(attrs);
    SetLogicalVolume(logVol);

    buildPlaceChannels();
}

ImpDetectorPayload::~ImpDetectorPayload()
{ }

void ImpDetectorPayload::buildPlaceChannels()
{
    static const G4double xy_offst = ImpHafxChannel::radiusInMm() + CRYSTAL_SEPARATION;

    G4cout << "building channels\n";G4cout.flush();
    for (size_t offset_idx = 0; offset_idx < NUM_CRYSTALS; ++offset_idx) {
        G4int offy = offset_idx / (NUM_CRYSTALS / 2);
        G4int offx = offset_idx % (NUM_CRYSTALS / 2);
        G4ThreeVector pos(
            -xy_offst + 2*offx*xy_offst,
            -xy_offst + 2*offy*xy_offst,
            SIDE_LENGTH/2 - DETECTOR_OFFSET);

        crystalChannels[offset_idx] = new ImpHafxChannel(
            nullptr, pos, GetLogicalVolume(), std::to_string(offset_idx));
    }
    G4cout << "done with channels\n";G4cout.flush();
}
