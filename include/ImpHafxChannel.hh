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
        ImpHafxChannel(
            G4RotationMatrix* rotMat, const G4ThreeVector& translate,
            G4LogicalVolume* motherLogVol, const G4String& channelId,
            const G4double attenuatorWindowThickness = 1 * mm);
        ~ImpHafxChannel();

        // as crystal becomes more fleshed out, update these
        G4double thickness()
        { return thicknessNoAttenuator() + attenuatorWindowThickness; }
        static G4double thicknessNoAttenuator()
        { return AL_HOUSING_DEPTH + DUMB_SI_THICKNESS; }
        static G4double diameter()
        { return WHOLE_DIAMETER; }
        static G4double radius()
        { return diameter() / 2; }

        void attachCrystalDetector();

    private:
        static G4OpticalSurface* tefSurf();

        void buildCrystal();

        void buildTeflonReflector();
        void attachTeflonOpticalSurface();

        void buildAlHousing();
        void buildAlAttenuator();
        void attachAlOpticalSurface(G4LogicalVolume* curAlLogVol);

        void buildQuartz();

        void buildBeryllium();
        void attachBeOpticalSurface();

        void buildDumbSi();
        void attachDumbSiOpticalSurface();

        G4ThreeVector cebr3AnchorCenter;
        G4ThreeVector quartzAnchorCenter;

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

        G4Tubs* alCylinder;
        G4LogicalVolume* alLogVol;
        G4PVPlacement* alPlacement;
        G4LogicalSkinSurface* alSkin;

        G4Tubs* attCylinder;
        G4LogicalVolume* attLogVol;
        G4PVPlacement* attPlacement;
        G4LogicalSkinSurface* attSkin;

        G4Tubs* qzCylinder;
        G4LogicalVolume* qzLogVol;
        G4PVPlacement* qzPlacement;
        // optical grease between quartz and crystal?
        
        G4Tubs* beCylinder;
        G4LogicalVolume* beLogVol;
        G4PVPlacement* bePlacement;
        G4LogicalSkinSurface* beSkin;

        G4Tubs* dumbSiCylinder;
        G4LogicalVolume* dumbSiLogVol;
        G4PVPlacement* dumbSiPlacement;
        G4LogicalSkinSurface* dumbSiSkin;

        G4String channelId;
        G4double attenuatorWindowThickness;

        static constexpr G4double BE_THICKNESS = 0.7 * mm;

        static constexpr G4double CEBR3_THICKNESS = 5 * mm;
        static constexpr G4double CEBR3_DIAMETER = 37 * mm;

        static constexpr G4double TEFLON_THICKNESS = 0.127 * mm;

        static constexpr G4double AL_HOUSING_THICKNESS = 1.25 * mm;
        static constexpr G4double AL_HOUSING_DEPTH = 11.3 * mm;

        // ???
        static constexpr G4double QUARTZ_THICKNESS = 3 * mm;
        static constexpr G4double QUARTZ_DIAMETER = 40.5 * mm;

        static constexpr G4double WHOLE_DIAMETER = 43 * mm;

        static constexpr G4double DUMB_SI_THICKNESS = 0.3 * mm;
};
