#pragma once

#include <array>
#include <memory>

#include "G4VUserDetectorConstruction.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Rotationmatrix.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"

#include "ImpHafxChannel.hh"
#include "ImpMaterials.hh"


class ImpDetectorConstruction : public G4VUserDetectorConstruction
{
    public:
        ImpDetectorConstruction();
        ~ImpDetectorConstruction();

        G4VPhysicalVolume* Construct() override;
        void ConstructSDandField() override;

    private:
        void constructEnclosing();
        void constructPlaceDetectors();

        G4Box* boundingBox;
        G4LogicalVolume* boundingBoxLogVol;
        G4VPhysicalVolume* boundingBoxPhysVol;

        std::array<ImpHafxChannel*, 4> crystalChannels;
        std::shared_ptr<G4RotationMatrix> crystalRotMat;

        const G4String kBOUNDING_BOX = "imp_enclosing_box";
        const G4String kBOUNDING_LOG_VOL = "imp_enclosing_log_vol";
        const G4String kBOUNDING_PHY_VOL = "imp_enclosing_phy_vol";
        static constexpr G4double CUBESAT_SIDE = 10 * cm;
        static constexpr G4double EXTRA_SPACE = 1 * cm;
        static constexpr G4int IMPRESS_Z_MUL = 3;
        static constexpr G4int IMPRESS_XY_MUL = 1;
};
