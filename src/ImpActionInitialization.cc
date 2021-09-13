#include "ImpActionInitialization.hh"

#include "ImpEventAction.hh"
#include "ImpWholeSatelliteConstruction.hh"
#include "ImpOnlyDetectorConstruction.hh"
#include "ImpPrimaryGeneratorAction.hh"
#include "ImpRunAction.hh"
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
    SetUserAction(new ImpPrimaryGeneratorAction);
    SetUserAction(new ImpTrackingAction);
}

/* void ImpActionInitialization::setupPrimaryGenerator() const */
/* { */
/*     using ct = ImpVDetectorConstruction::ConstructionType; */
/*     if (dc->detectorConstructionType() == ct::wholeSatellite) { */
/*         const auto* whole = static_cast<ImpWholeSatelliteConstruction*>(dc); */
/*         SetUserAction(new ImpPrimaryGeneratorAction(whole->peekBoundingBox(), flareSize)); */
/*     } */
/*     else if (dc->detectorConstructionType() == ct::onlyDetector) { */
/*         const auto* onlyDet = static_cast<ImpOnlyDetectorConstruction*>(dc); */
/*         SetUserAction(new ImpPrimaryGeneratorAction(onlyDet->peekBoundingCyl(), flareSize)); */
/*     } */
/* } */
