#include "ImpTempLogVol.hh"

namespace {
    G4LogicalVolume* temp = nullptr;
}

// used for subclassing G4PVPlacement
G4LogicalVolume* tempLogVol()
{
    if (temp == nullptr) {
        temp = new G4LogicalVolume(
            new G4Box("", 1, 1, 1),
            G4Material::GetMaterial(ImpMaterials::kVACUUM), "");
    }
    return temp;
}
