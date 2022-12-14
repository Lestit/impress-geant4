#pragma once

#include <array>
#include <memory>

#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4Material.hh>
#include <G4PVPlacement.hh>
#include <G4VPhysicalVolume.hh>
#include <G4RotationMatrix.hh>
#include <G4SystemOfUnits.hh>
#include <G4VisAttributes.hh>

#include <ImpVDetectorConstruction.hh>
#include <ImpDetectorPayload.hh>
#include <ImpMaterials.hh>


class ImpWholeSatelliteConstruction : public ImpVDetectorConstruction
{
    public:
        ImpWholeSatelliteConstruction();
        ~ImpWholeSatelliteConstruction();

        G4VPhysicalVolume* Construct() override;
        void ConstructSDandField() override;

        ConstructionType detectorConstructionType() const override
        { return ConstructionType::wholeSatellite; }

        const G4Box* peekBoundingBox() const
        { return boundingBox; }

    private:
        void constructEnclosing();
        void constructPlaceDetectors();

        G4Box* boundingBox;
        G4LogicalVolume* boundingBoxLogVol;
        G4VPhysicalVolume* boundingBoxPhysVol;

        ImpDetectorPayload* payload;


        const G4String kBOUNDING_BOX = "imp_bounding_box";
        const G4String kBOUNDING_LOG_VOL = "imp_bounding_log_vol";
        const G4String kBOUNDING_PHY_VOL = "imp_bounding_phy_vol";
        static constexpr G4double CUBESAT_SIDE = 10 * cm;
        static constexpr G4double EXTRA_SPACE = 2 * cm;

        // IMPRESS is a 3U cubesat
        static constexpr G4int IMPRESS_UNITS_LONG = 3;
        static constexpr G4int IMPRESS_UNITS_WIDE = 1;
};
