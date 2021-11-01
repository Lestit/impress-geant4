#pragma once

#include <array>
#include <string>

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"

#include "ImpHafxChannel.hh"

class G4Box;
class G4LogicalVolume;
class G4SubtractionSolid;

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
        static constexpr G4double X123_PEEP_DIAM = 14.22 * mm;
        static constexpr G4double CRYSTAL_HOLE_DIAM = 43 * mm;
        // cubesat standard
        static constexpr G4double SIDE_LENGTH = 10 * cm;

        // top panel
        static constexpr G4double TOP_PANEL_SIDE = 98 * mm;
        static constexpr G4double TOP_PANEL_THICK = 4.5 * mm;
        static constexpr G4double CRYSTAL_HOLE_SPACING = TOP_PANEL_SIDE / 4;
        // 1mm < crystal diameter
        static constexpr G4double CRYSTAL_ENTRANCE_DIAM = 43 * mm;//42 * mm;
        G4SubtractionSolid* topPanelSubSol;
        G4PVPlacement* topPanelPlacement;

        void buildPlaceChannels();
        void buildPlaceTopPanel();

        static const size_t NUM_CRYSTALS = 4;
        std::array<ImpHafxChannel*, NUM_CRYSTALS> crystalChannels;
};
