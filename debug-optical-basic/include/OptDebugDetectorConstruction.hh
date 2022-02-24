#pragma once

#include "G4SystemOfUnits.hh"
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4Box;
class G4VPhysicalVolume;
class G4LogicalVolume;

class OptDebugDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    OptDebugDetectorConstruction();
    virtual ~OptDebugDetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
  private:
    void makeCeBr3();
    void makeTeflon();
    void makeAluminum();
    G4VPhysicalVolume* physWorld;
    G4LogicalVolume* logicWorld;

    constexpr static G4double CRYST_SIZE = 2 * cm;
    constexpr static G4double TEF_THICK = 1 * mm;
    constexpr static G4double AL_THICK = 1 * mm;
    constexpr static G4double AIR_GAP = 10 * um;
};
