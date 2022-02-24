#pragma once

#include "FTFP_BERT.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4OpticalParameters.hh"
#include "G4OpticalPhysics.hh"
    
class OptDebugPhysicsList : public FTFP_BERT
{ 
    public:
        OptDebugPhysicsList();
        virtual ~OptDebugPhysicsList() override;
    private:

};

