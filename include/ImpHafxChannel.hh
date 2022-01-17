#pragma once

#include <string>
#include <ChannelConstants.hh>
#include "G4Cache.hh"
#include "G4PhysicalConstants.hh"
#include "G4PVPlacement.hh"
#include "G4String.hh"
#include <G4SystemOfUnits.hh>
static constexpr G4double inch = 25.4 * mm;

class G4Box;
class G4LogicalSkinSurface;
class G4LogicalVolume;
class G4OpticalSurface;
class G4Tubs;
class G4SubtractionSolid;
class G4UnionSolid;

class ImpScintCrystalSensitiveDetector;
class ImpSiSensitiveDetector;

class ImpHafxChannel : public G4PVPlacement
{
    public:
        ImpHafxChannel(
            G4RotationMatrix* rotMat, const G4ThreeVector& translate,
            G4LogicalVolume* motherLogVol, const G4String& channelId,
            const G4double attenuatorWindowThickness = 1 * mm);
        ~ImpHafxChannel();

        // as crystal becomes more fleshed out, update these
        static G4double thicknessNoAttenuator()
        { return AL_HOUSING_DEPTH + LIGHT_GUIDE_THICKNESS + SI_THICKNESS; }
        static G4double diameter()
        { return WHOLE_DIAMETER; }
        static G4double radius()
        { return diameter() / 2; }

        G4double thickness() const
        { return thicknessNoAttenuator() + attenuatorWindowThickness; }
        const std::string& peekSensitiveDetectorName() const
        { return sensDetName; }

        void attachCrystalDetector();
        void attachSiDetector();
        void attachAllDetectors();

    private:
        static G4OpticalSurface* tefSurf();

        void buildCrystal();
        void attachCeBr3OpticalSurface();

        void buildTeflonReflector();
        void attachTeflonOpticalSurface(G4LogicalVolume* tlv);

        void buildAlHousing();
        void buildAlAttenuator();
        void attachAlOpticalSurface(G4LogicalVolume* curAlLogVol);

        void buildQuartz();
        void attachQuartzOpticalSurface();

        void buildBeryllium();
        void attachBeOpticalSurface();

        void buildPaint();
        void attachPaintBoundarySurface(G4VPhysicalVolume* ppv, G4LogicalVolume* plv);
        void buildLightGuide();

        void buildSi();
        void attachSiOpticalSurface();

        G4ThreeVector cebr3AnchorCenter;
        G4ThreeVector quartzAnchorCenter;

        G4Tubs* boundingCylinder;

        G4Tubs* crystalCylinder;
        G4LogicalVolume* crystalLogVol;
        G4PVPlacement* crystalPlacement;
        // G4Cache ensures thread safety
        G4Cache<ImpScintCrystalSensitiveDetector*> crystalSensDet;
        G4Cache<ImpSiSensitiveDetector*> siSensDet;

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
        G4LogicalSkinSurface* qzSkin;
        // optical grease between quartz and crystal?
        
        G4Tubs* beCylinder;
        G4LogicalVolume* beLogVol;
        G4PVPlacement* bePlacement;
        G4LogicalSkinSurface* beSkin;

        G4Box* lightGuideBox;
        G4LogicalVolume* lightGuideLogVol;
        G4PVPlacement* lightGuidePlacement;
        G4Box* lightGuideWrapToBeCutoutBox;
        G4UnionSolid* lightGuideWrapSolid;
        G4LogicalVolume* lightGuideWrapLogVol;
        G4PVPlacement* lightGuideWrapPlacement;

        static constexpr size_t NUM_SIPMS = 16;
        std::array<G4Box*, NUM_SIPMS> siBoxes;
        std::array<G4LogicalVolume*, NUM_SIPMS> siLogVols;
        /* G4PVPlacement* siPlacement; */
        /* G4LogicalSkinSurface* siSkin; */

        G4String channelId;
        std::string sensDetName;
        G4double attenuatorWindowThickness;

        static constexpr G4double WHOLE_DIAMETER = 43 * mm;

        static constexpr G4double BE_THICKNESS = 0.7 * mm;

        static constexpr G4double CEBR3_THICKNESS = 5 * mm;
        static constexpr G4double CEBR3_DIAMETER = 37 * mm;

        static constexpr G4double TEFLON_THICKNESS = 0.127 * mm;

        static constexpr G4double AL_HOUSING_THICKNESS = 1.25 * mm;
        static constexpr G4double AL_HOUSING_DEPTH = 11.3 * mm;

        // ???
        static constexpr G4double QUARTZ_THICKNESS = 3 * mm;
        static constexpr G4double QUARTZ_DIAMETER = 40.5 * mm;

        // to be updated
        static constexpr G4double LIGHT_GUIDE_THICKNESS = 0.25 * inch;
        // guess for now
        static constexpr G4double LIGHT_GUIDE_SIDE_LENGTH = 1 * inch;

        static constexpr G4double SI_THICKNESS = 0.3 * mm;
        static constexpr G4double BROADCOM_FULL_LENGTH = 3.88 * mm;
        static constexpr G4double SI_SPACING = 0.25 * mm;
        static constexpr G4double SI_SIDE_LENGTH = 3*SI_SPACING + 4*BROADCOM_FULL_LENGTH;
};
