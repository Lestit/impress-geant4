#pragma once

#include "G4VUserActionInitialization.hh"

/// Action initialization class.

class OptDebugActionInitialization : public G4VUserActionInitialization
{
  public:
    OptDebugActionInitialization();
    virtual ~OptDebugActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;
};
