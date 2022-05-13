#include "ImpPhysicsList.hh"

#include <G4StepLimiterPhysics.hh>
#include <G4EmStandardPhysics_option4.hh>
#include <G4OpticalParameters.hh>
#include <G4OpticalPhysics.hh>
#include <G4RadioactiveDecayPhysics.hh>

ImpPhysicsList::ImpPhysicsList() :
    FTFP_BERT()
{
    // updates to FTFP_BERT lists
    ReplacePhysics(new G4EmStandardPhysics_option4);
    RegisterPhysics(new G4OpticalPhysics);
    RegisterPhysics(new G4StepLimiterPhysics);
    RegisterPhysics(new G4RadioactiveDecayPhysics);
} 

ImpPhysicsList::~ImpPhysicsList()
{ }
