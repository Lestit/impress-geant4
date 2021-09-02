#include "impress.hh"

int main(int argc, char* argv[])
{
    G4UIExecutive* uiExec = nullptr;
    bool interactive = (argc == 1);
    if (interactive) {
        uiExec = new G4UIExecutive(argc, argv);
    }

    // random engine
    // no customization for now?

    auto* runMan = G4RunManagerFactory::CreateRunManager(G4RunManagerType::MTOnly);
    // assumes hyperthreading (2 threads/core)
    runMan->SetNumberOfThreads(2 * G4Threading::G4GetNumberOfCores());

    runMan->SetUserInitialization(new ImpPhysicsList);
    auto* detCon = new ImpDetectorConstruction;
    runMan->SetUserInitialization(detCon);
    runMan->SetUserInitialization(new ImpActionInitialization(detCon));

    G4VisManager* visMan = new G4VisExecutive;
    visMan->Initialize();
    auto* uiMan = G4UImanager::GetUIpointer();

    if (interactive) {
        uiMan->ApplyCommand("/control/execute macros/vis.mac");
        uiExec->SessionStart();
        delete uiExec;
    }
    else {
        std::stringstream ss;
        ss << "/control/execute " << argv[1];
        uiMan->ApplyCommand(ss.str());
    }

    delete visMan;
    delete runMan;
    return 0;
}
