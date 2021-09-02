#include "ImpActionInitialization.hh"

ImpActionInitialization::ImpActionInitialization(const ImpDetectorConstruction* con) :
    G4VUserActionInitialization(),
    detCon(con)
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
}
