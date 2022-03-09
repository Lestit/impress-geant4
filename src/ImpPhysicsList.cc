#include "ImpPhysicsList.hh"

#include <G4StepLimiterPhysics.hh>

ImpPhysicsList::ImpPhysicsList() :
    FTFP_BERT()
{
    // updates to FTFP_BERT lists
    ReplacePhysics(new G4EmStandardPhysics_option4);
    RegisterPhysics(new G4OpticalPhysics);
    RegisterPhysics(new G4StepLimiterPhysics);
} 

ImpPhysicsList::~ImpPhysicsList()
{ }
