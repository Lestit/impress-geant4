#pragma once

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class OptDebugEventAction;

class G4LogicalVolume;

class OptDebugSteppingAction : public G4UserSteppingAction
{
  public:
    OptDebugSteppingAction(OptDebugEventAction* eventAction);
    virtual ~OptDebugSteppingAction();

    // method from the base class
    virtual void UserSteppingAction(const G4Step*);

  private:
    OptDebugEventAction*  fEventAction;
    /* G4LogicalVolume* fScoringVolume; */
};
