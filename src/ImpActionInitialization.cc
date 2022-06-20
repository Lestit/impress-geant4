#include "ImpActionInitialization.hh"

#include "ImpEventAction.hh"
#include "ImpWholeSatelliteConstruction.hh"
#include "ImpOnlyDetectorConstruction.hh"
#include "ImpPrimaryGeneratorAction.hh"
#include "ImpRunAction.hh"
#include "ImpSteppingAction.hh"
#include "ImpTrackingAction.hh"
#include "ImpVDetectorConstruction.hh"

ImpActionInitialization::ImpActionInitialization()
        : G4VUserActionInitialization()
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
    SetUserAction(new ImpRunAction);
    SetUserAction(new ImpTrackingAction);
    SetUserAction(new ImpSteppingAction);
    setupPrimaryGenerator();
}

void ImpActionInitialization::setupPrimaryGenerator() const
{
    SetUserAction(new ImpPrimaryGeneratorAction);
}
