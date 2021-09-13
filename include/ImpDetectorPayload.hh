#pragma once

#include <array>
#include <string>

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"

#include "ImpHafxChannel.hh"

class G4Box;
class G4LogicalVolume;

class ImpDetectorPayload : public G4PVPlacement
{
    public:
        ImpDetectorPayload(G4RotationMatrix* rotMat, const G4ThreeVector& translate, G4LogicalVolume* motherLogVol);
        ~ImpDetectorPayload();

        static G4double sideLengthCm()
        { return SIDE_LENGTH; }

        void constructSensitiveDetectors();

    private:
        static constexpr G4double CRYSTAL_SEPARATION = 6 * mm;
        // cubesat standard
        static constexpr G4double SIDE_LENGTH = 10 * cm;
        static constexpr G4double DETECTOR_OFFSET = 7.5 * mm;

        void buildPlaceChannels();
        void buildPlaceTopPanel();

        //G4Box* boundingBox;

        static const size_t NUM_CRYSTALS = 4;
        std::array<ImpHafxChannel*, NUM_CRYSTALS> crystalChannels;
};
