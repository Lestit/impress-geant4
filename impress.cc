#include "impress.hh"

int main(int argc, char* argv[])
{
    // random engine
    // no customization for now?

    auto* runMan = new G4MTRunManager;
    // assumes hyperthreading (2 threads/core)
    runMan->SetNumberOfThreads(2 * G4Threading::G4GetNumberOfCores());

    auto* physList = new ImpPhysicsList;
    physList->SetVerboseLevel(0);
    runMan->SetUserInitialization(physList);
    runMan->SetUserInitialization(new ImpDetectorConstruction);
    runMan->SetUserInitialization(new ImpActionInitialization);

    auto* uiMan = G4UImanager::GetUIpointer();

    G4OpticalParameters::Instance()->SetProcessActivation("Scintillation", false);

    bool interactive = (argc == 1);
    if (interactive) {
        auto* visMan = new G4VisExecutive;
        visMan->Initialize();
        auto* uiExec = new G4UIExecutive(argc, argv);
        uiMan->ApplyCommand("/control/execute macros/init_gui.mac");
        uiExec->SessionStart();
        delete uiExec;
        delete visMan;
    }
    else {
        uiMan->ApplyCommand("/run/initialize");
        std::stringstream ss;
        ss << "/control/execute " << argv[1];
        uiMan->ApplyCommand(ss.str());
    }

    /* if (ImpAnalysis::instance()) */
    /*     ImpAnalysis::instance->quit(); */
    delete runMan;
    return 0;
}
