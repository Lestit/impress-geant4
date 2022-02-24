#include "OptDebugActionInitialization.hh"
#include "OptDebugPrimaryGeneratorAction.hh"
#include "OptDebugRunAction.hh"
#include "OptDebugEventAction.hh"
#include "OptDebugSteppingAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OptDebugActionInitialization::OptDebugActionInitialization()
 : G4VUserActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OptDebugActionInitialization::~OptDebugActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OptDebugActionInitialization::BuildForMaster() const
{
  OptDebugRunAction* runAction = new OptDebugRunAction;
  SetUserAction(runAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OptDebugActionInitialization::Build() const
{
  SetUserAction(new OptDebugPrimaryGeneratorAction);

  OptDebugRunAction* runAction = new OptDebugRunAction;
  SetUserAction(runAction);
  
  OptDebugEventAction* eventAction = new OptDebugEventAction(runAction);
  SetUserAction(eventAction);
  
  SetUserAction(new OptDebugSteppingAction(eventAction));
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
