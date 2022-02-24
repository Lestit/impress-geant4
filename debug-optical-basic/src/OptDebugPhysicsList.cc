#include "OptDebugPhysicsList.hh"

OptDebugPhysicsList::OptDebugPhysicsList() :
    FTFP_BERT()
{
    // updates to FTFP_BERT lists
    ReplacePhysics(new G4EmStandardPhysics_option4);
    RegisterPhysics(new G4OpticalPhysics);
} 
    
OptDebugPhysicsList::~OptDebugPhysicsList()
{ }   
