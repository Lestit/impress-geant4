#include "ImpPhysicsList.hh"

ImpPhysicsList::ImpPhysicsList() :
    FTFP_BERT()
{
    // updates to FTFP_BERT lists
    ReplacePhysics(new G4EmStandardPhysics_option4);
    RegisterPhysics(new G4OpticalPhysics);
} 

ImpPhysicsList::~ImpPhysicsList()
{ }
