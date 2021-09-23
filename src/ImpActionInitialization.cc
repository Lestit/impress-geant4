#include "ImpActionInitialization.hh"

#include "ImpEventAction.hh"
#include "ImpWholeSatelliteConstruction.hh"
#include "ImpOnlyDetectorConstruction.hh"
#include "ImpPrimaryGeneratorAction.hh"
#include "ImpRunAction.hh"
#include "ImpTrackingAction.hh"
#include "ImpVDetectorConstruction.hh"

ImpActionInitialization::ImpActionInitialization()
        : G4VUserActionInitialization(),
    dc(nullptr)
{ }

ImpActionInitialization::ImpActionInitialization(const ImpVDetectorConstruction* dc) : G4VUserActionInitialization(),
    dc(dc)
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
    setupPrimaryGenerator();
}

void ImpActionInitialization::setupPrimaryGenerator() const
{
    if (dc == nullptr) {
        SetUserAction(new ImpPrimaryGeneratorAction);
        return;
    }

    auto* pga = new ImpPrimaryGeneratorAction;
    using ct = ImpVDetectorConstruction::ConstructionType;
    if (dc->detectorConstructionType() == ct::wholeSatellite) {
        const auto* whole = static_cast<const ImpWholeSatelliteConstruction*>(dc);
        pga->updateWorldVolume(whole->peekBoundingBox());
        SetUserAction(pga);
    }
    else if (dc->detectorConstructionType() == ct::onlyDetector) {
        const auto* onlyDet = static_cast<const ImpOnlyDetectorConstruction*>(dc);
        pga->updateWorldVolume(onlyDet->peekBoundingCyl());
        SetUserAction(pga);
    }
}
