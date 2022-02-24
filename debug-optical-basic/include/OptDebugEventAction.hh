#pragma once

#include "G4UserEventAction.hh"
#include "globals.hh"

class OptDebugRunAction;

/// Event action class
///

class OptDebugEventAction : public G4UserEventAction
{
  public:
    OptDebugEventAction(OptDebugRunAction* runAction);
    virtual ~OptDebugEventAction();

    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);

    void AddEdep(G4double edep) { fEdep += edep; }

  private:
    OptDebugRunAction* fRunAction;
    G4double     fEdep;
};
