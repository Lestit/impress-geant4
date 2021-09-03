#pragma once

#include <string>

#include "G4AutoLock.hh"
#include "G4Box.hh"
#include "G4Color.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PhysicalConstants.hh"
#include "G4GenericPolyCone.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4String.hh"
#include "G4SystemOfUnits.hh"
#include "G4Threading.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4VisAttributes.hh"

#include "ImpMaterials.hh"
#include "ImpTempLogVol.hh"

class ImpHafxChannel : public G4PVPlacement
{
    public:
        ImpHafxChannel(G4RotationMatrix* rotMat, const G4ThreeVector& translate, G4LogicalVolume* motherLogVol, const G4String& channelId);
        ~ImpHafxChannel();

        // as crystal becomes more fleshed out, update these
        static G4double thicknessInMm()
        { return CEBR3_THICKNESS + TEFLON_THICKNESS; }
        static G4double diameterInMm()
        { return CEBR3_DIAMETER + 2*TEFLON_THICKNESS; }
        static G4double radiusInMm()
        { return diameterInMm() / 2; }

    private:
        void buildCrystal();
        void buildTeflonReflector();

        G4ThreeVector adjustPlacements;

        G4Tubs* boundingCylinder;

        G4Tubs* crystalCylinder;
        G4LogicalVolume* crystalLogVol;
        G4PVPlacement* crystalPlacement;

        G4Tubs* tefRing;
        G4Tubs* tefCap;
        G4LogicalVolume* tefRingLogVol;
        G4LogicalVolume* tefCapLogVol;
        G4PVPlacement* tefRingPlacement;
        G4PVPlacement* tefCapPlacement;

        G4String channelId;

        static constexpr G4double CEBR3_THICKNESS = 5 * mm;
        static constexpr G4double CEBR3_DIAMETER = 37 * mm;

        static constexpr G4double TEFLON_THICKNESS = 1.75 * mm;
};
