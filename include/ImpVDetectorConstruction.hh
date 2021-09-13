#pragma once

#include "G4VUserDetectorConstruction.hh"
#include "ImpMaterials.hh"

class ImpVDetectorConstruction : public G4VUserDetectorConstruction
{
    public:
        ImpVDetectorConstruction()
        { ImpMaterials::makeMaterials(); }
        enum class ConstructionType { wholeSatellite, onlyDetector };

        virtual ConstructionType detectorConstructionType() const =0;
};
