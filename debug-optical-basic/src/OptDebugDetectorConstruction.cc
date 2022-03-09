#include "OptDebugDetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4Orb.hh"
#include "G4OpticalSurface.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"

#include "OptDebugMaterials.hh"

// anonymous namespace so other files can't access these
namespace {
  G4bool checkOverlaps = true;
  void attachTeflonOpticalSurf(G4LogicalVolume* lv)
  {
    static G4OpticalSurface* ts = nullptr;
    if (ts == nullptr) {
      ts = new G4OpticalSurface("teflon_optical_surf");
      ts->SetType(dielectric_dielectric);
      ts->SetModel(unified);
      ts->SetFinish(groundfrontpainted);
      ts->SetSigmaAlpha(0.);
      ts->SetMaterialPropertiesTable(
          G4NistManager::Instance()->
          FindOrBuildMaterial(OptDebugMaterials::kNIST_TEFLON)->
          GetMaterialPropertiesTable());
    }

    (void) new G4LogicalSkinSurface("teflon_skin_surf", lv, ts);
  }

  void attachAlOpticalSurface(G4LogicalVolume* lv)
  {
    static G4OpticalSurface* as = nullptr;
    if (as == nullptr) {
      as = new G4OpticalSurface("al_optical_surf");
      as->SetModel(unified);
      as->SetType(dielectric_metal);
      as->SetFinish(polished);
      as->SetMaterialPropertiesTable(
        G4Material::GetMaterial(OptDebugMaterials::kAL)->
        GetMaterialPropertiesTable());
    }
    (void) new G4LogicalSkinSurface("al_skin_surf", lv, as);
  }
}

OptDebugDetectorConstruction::OptDebugDetectorConstruction()
: G4VUserDetectorConstruction(), physWorld(nullptr), logicWorld(nullptr)
{ OptDebugMaterials::makeMaterials(); }

OptDebugDetectorConstruction::~OptDebugDetectorConstruction()
{ }

G4VPhysicalVolume* OptDebugDetectorConstruction::Construct()
{  
  G4VisAttributes va;

  G4Material* world_mat = G4Material::GetMaterial(OptDebugMaterials::kVACUUM);
  const G4double worldSize = 5*cm;
  G4Box* solidWorld =    
    new G4Box("World",
       0.5*worldSize, 0.5*worldSize, 0.5*worldSize);
      
  logicWorld = new G4LogicalVolume(
      solidWorld, world_mat, "World");
  va.SetColor(1, 1, 1, 0.05);
  va.SetVisibility(true);
  logicWorld->SetVisAttributes(va);

  physWorld = 
    new G4PVPlacement(
      nullptr, G4ThreeVector(), logicWorld,
      "World", nullptr, false, 0, checkOverlaps);
 
  makeCeBr3();
  bool doMakeTeflon = false;
  if (doMakeTeflon) makeTeflon();
  bool doMakeAluminum = true;
  if (doMakeAluminum) makeAluminum();
  
  return physWorld;
}

void OptDebugDetectorConstruction::makeCeBr3()
{
  G4VisAttributes va;
  auto* cebr3 = G4Material::GetMaterial(OptDebugMaterials::kCEBR3);

  auto* crystBox = new G4Box(
    "cebr3_box", 0.5*CRYST_SIZE, 0.5*CRYST_SIZE, 0.5*CRYST_SIZE);

  auto* crystLv = new G4LogicalVolume(
    crystBox, cebr3, "cebr3_log");
  va.SetColor(0.35, 0.5, 0.92, 0.8);
  va.SetVisibility(true);
  crystLv->SetVisAttributes(va);

  auto* crystPlace = new G4PVPlacement(
    nullptr, G4ThreeVector(), crystLv,
    "cebr3_phys", logicWorld, false, 0, checkOverlaps);
}

void OptDebugDetectorConstruction::makeTeflon()
{
  G4VisAttributes va;

  const G4double precutHalfThick = 0.5 * (AIR_GAP + TEF_THICK + CRYST_SIZE);

  auto* ptfe = G4NistManager::Instance()->FindOrBuildMaterial(OptDebugMaterials::kNIST_TEFLON);
  auto* preCutPtfeBox = new G4Box(
    "precut_ptfe", precutHalfThick, precutHalfThick, precutHalfThick);

  auto addAirGapHalfXyz = 0.5 * (CRYST_SIZE + AIR_GAP);
  auto* crystPlusAirGap = new G4Box(
    "", addAirGapHalfXyz, addAirGapHalfXyz, addAirGapHalfXyz);
  G4ThreeVector translateSubtract(0, 0, 0);
  auto* slicedPtfe = new G4SubtractionSolid(
    "sliced_ptfe", preCutPtfeBox, crystPlusAirGap, nullptr, translateSubtract);

  translateSubtract = G4ThreeVector(0, 0, 1.8 * cm);
  auto* cutCap = new G4Box("", precutHalfThick + 5*mm, precutHalfThick + 5*mm, precutHalfThick);
  auto* slicedPtfeOpen = new G4SubtractionSolid(
    "sliced_ptfe_open", slicedPtfe, cutCap, nullptr, translateSubtract);

  auto* ptfeLv = new G4LogicalVolume(
    slicedPtfeOpen, ptfe, "ptfe_log");
  va.SetColor(0, 1, 0, 0.1);
  va.SetVisibility(true);
  ptfeLv->SetVisAttributes(va);
  attachTeflonOpticalSurf(ptfeLv);

  auto* ptfePlace = new G4PVPlacement(
    nullptr, G4ThreeVector(), ptfeLv,
    "ptfe_phys", logicWorld, false, 0, checkOverlaps);
}

void OptDebugDetectorConstruction::makeAluminum()
{
  G4VisAttributes va;
  const G4double sliceCapThick = 0.5 * (TEF_THICK + CRYST_SIZE + AL_THICK + 2*AIR_GAP) + 5*mm;
  const G4double precutHalfThick = sliceCapThick - 5*mm;
  const G4double sliceOutHalfThick = precutHalfThick - 0.5 * AL_THICK;

  auto* al = G4Material::GetMaterial(OptDebugMaterials::kAL);
  auto* precut = new G4Box("", precutHalfThick, precutHalfThick, precutHalfThick);
  auto* cutThisOut = new G4Box("", sliceOutHalfThick, sliceOutHalfThick, sliceOutHalfThick);
  auto* cutOffEnd = new G4Box("", sliceCapThick, sliceCapThick, 6*mm);
  auto* hollow = new G4SubtractionSolid(
    "hollow_al", precut, cutThisOut, nullptr, G4ThreeVector());

  G4ThreeVector translateSubtract(0, 0, sliceCapThick);
  auto* open = new G4SubtractionSolid(
    "hollow_open_al", hollow, cutOffEnd, nullptr, translateSubtract);
  
  auto* alLv = new G4LogicalVolume(open, al, "al_log");
  va.SetColor(1, 0., 0., 0.1);
  va.SetVisibility(true);
  alLv->SetVisAttributes(va);
  attachAlOpticalSurface(alLv);

  auto* alPlace = new G4PVPlacement(
    nullptr, G4ThreeVector(), alLv,
    "al_phys", logicWorld, false, 0, checkOverlaps);
}
