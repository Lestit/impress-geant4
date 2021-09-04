#include "ImpActionInitialization.hh"

#include "ImpEventAction.hh"
#include "ImpDetectorConstruction.hh"
#include "ImpPrimaryGeneratorAction.hh"
#include "ImpRunAction.hh"
#include "ImpTrackingAction.hh"

ImpActionInitialization::ImpActionInitialization() :
    G4VUserActionInitialization()
{ }


ImpActionInitialization::~ImpActionInitialization()
{ }


void ImpActionInitialization::BuildForMaster() const
{
    SetUserAction(new ImpRunAction);
}

void ImpActionInitialization::Build() const
{
    SetUserAction(new ImpEventAction);
    SetUserAction(new ImpPrimaryGeneratorAction);
    SetUserAction(new ImpRunAction);
    SetUserAction(new ImpTrackingAction);
}
