#pragma once

#include <ImpVDetectorConstruction.hh>

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Tubs;
class ImpHafxChannel;

class ImpOnlyDetectorConstruction : public ImpVDetectorConstruction
{
    public:
        ImpOnlyDetectorConstruction() =delete;
        ImpOnlyDetectorConstruction(const G4String& flareSize);
        ~ImpOnlyDetectorConstruction();

        G4VPhysicalVolume* Construct() override;
        void ConstructSDandField() override;

        ConstructionType detectorConstructionType() const override
        { return ConstructionType::onlyDetector; }

        const G4Tubs* peekBoundingCyl() const
        { return boundingCylinder; }
    
    private:
        void constructEnclosing();
        void constructPlaceDetector();

        G4String flareSize;

        G4Tubs* boundingCylinder;
        ImpHafxChannel* singleDetector;
        G4LogicalVolume* boundingLogVol;
        G4VPhysicalVolume* boundingPhysVol;

        static constexpr G4double WORLD_DEPTH = 2 * cm;
};
