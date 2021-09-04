#pragma once

#include <string>

#include "G4Cache.hh"
#include "G4PhysicalConstants.hh"
#include "G4PVPlacement.hh"
#include "G4String.hh"
#include "G4SystemOfUnits.hh"

class G4LogicalSkinSurface;
class G4LogicalVolume;
class G4OpticalSurface;
class G4Tubs;
class G4UnionSolid;

class ImpScintCrystalSensitiveDetector;

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

        void attachCrystalDetector();

    private:
        static G4OpticalSurface* tefSurf();

        void buildCrystal();
        void buildTeflonReflector();
        void attachTeflonOpticalSurface();

        G4ThreeVector adjustPlacements;

        G4Tubs* boundingCylinder;

        G4Tubs* crystalCylinder;
        G4LogicalVolume* crystalLogVol;
        G4PVPlacement* crystalPlacement;
        // G4Cache ensures thread safety
        G4Cache<ImpScintCrystalSensitiveDetector*> crystalSensDet;

        G4UnionSolid* tefSolid;
        G4LogicalVolume* tefLogVol;
        G4PVPlacement* tefPlacement;
        G4LogicalSkinSurface* tefSkin;

        G4String channelId;

        static constexpr G4double CEBR3_THICKNESS = 5 * mm;
        static constexpr G4double CEBR3_DIAMETER = 37 * mm;

        static constexpr G4double TEFLON_THICKNESS = 1.75 * mm;
};
