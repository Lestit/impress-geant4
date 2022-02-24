#pragma once

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

class G4ParticleGun;
class G4GeneralParticleSource;
class G4Event;
class G4Box;

class OptDebugPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    OptDebugPrimaryGeneratorAction();    
    virtual ~OptDebugPrimaryGeneratorAction();

    // method from the base class
    virtual void GeneratePrimaries(G4Event*);         
  
  private:
    G4GeneralParticleSource*  gps;
};
