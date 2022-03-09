#pragma once

#include <any>
#include <memory>
#include <string>

#include <ChannelConstants.hh>
#include <ImpGlobalConfigs.hh>

#include "G4Cache.hh"
#include "G4PhysicalConstants.hh"
#include "G4PVPlacement.hh"
#include "G4String.hh"

class G4Box;
class G4LogicalSkinSurface;
class G4LogicalVolume;
class G4OpticalSurface;
class G4Tubs;
class G4SubtractionSolid;
class G4UnionSolid;
class G4UserLimits;

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
        {
            auto crystalThick = ImpGlobalConfigs::instance().configOption<double>(
                ImpGlobalConfigs::kCEBR3_LENGTH) * mm;
            return crystalThick + AL_DEPTH_DELTA + LIGHT_GUIDE_THICKNESS + SI_THICKNESS;
        }
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

        void finishCrystalEdges();
        std::unordered_map<std::string, G4VPhysicalVolume*> fillerCrystalBorderPhysicalVols();

        G4ThreeVector cebr3AnchorCenter;
        G4ThreeVector quartzAnchorCenter;

        G4Tubs* boundingCylinder;

        G4Tubs* crystalCylinder;
        G4LogicalVolume* crystalLogVol;
        G4PVPlacement* crystalPlacement;
        // G4Cache ensures thread safety
        G4Cache<ImpScintCrystalSensitiveDetector*> crystalSensDet;
        G4Cache<ImpSiSensitiveDetector*> siSensDet;

        /* G4UnionSolid* tefSolid; */
        /* G4LogicalVolume* tefRingPlacement; // just the ring around the crystal */
        G4LogicalVolume* tefLogVol;
        /* G4PVPlacement* tefPlacement; */
        G4PVPlacement* tefRingPlacement;
        G4PVPlacement* tefCapPlacement;

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

        G4UserLimits* uLims;

        static constexpr std::size_t NUM_SIPMS = 16;
        std::array<G4Box*, NUM_SIPMS> siBoxes;
        std::array<G4LogicalVolume*, NUM_SIPMS> siLogVols;

        G4String channelId;
        std::string sensDetName;
        G4double attenuatorWindowThickness;
        G4double cebr3Thickness;
        G4double teflonAirGap;
        G4double alHousingDepth;
};
