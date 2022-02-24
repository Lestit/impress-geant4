#include "OptDebugDetectorConstruction.hh"
#include "OptDebugActionInitialization.hh"
#include "OptDebugPhysicsList.hh"

#include "G4RunManagerFactory.hh"

#include "G4UImanager.hh"
#include "QBBC.hh"
#include "FTFP_BERT.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "Randomize.hh"
#include "G4OpticalParameters.hh"

int main(int argc,char** argv)
{
  G4UIExecutive* ui = nullptr;
  if ( argc == 1 ) {
    ui = new G4UIExecutive(argc, argv);
  }

  auto* runManager =
    G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

  runManager->SetUserInitialization(new OptDebugDetectorConstruction());

  auto* pl = new FTFP_BERT;
  pl->ReplacePhysics(new G4EmStandardPhysics_option4);
  pl->RegisterPhysics(new G4OpticalPhysics);
  runManager->SetUserInitialization(pl);
    
  runManager->SetUserInitialization(new OptDebugActionInitialization());
  
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();

  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  G4OpticalParameters::Instance()->SetProcessActivation("Scintillation", true);

  if ( ! ui ) { 
    // batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }
  else { 
    UImanager->ApplyCommand("/control/execute init.mac");
    ui->SessionStart();
    delete ui;
  }
  
  delete visManager;
  delete runManager;
}
