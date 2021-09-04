#include "impress.hh"

int main(int argc, char* argv[])
{
    // random engine
    // no customization for now?

    auto* runMan = new G4MTRunManager;
    // assumes hyperthreading (2 threads/core)
    runMan->SetNumberOfThreads(2 * G4Threading::G4GetNumberOfCores());

    runMan->SetUserInitialization(new ImpPhysicsList);
    auto* detCon = new ImpDetectorConstruction;
    runMan->SetUserInitialization(detCon);
    runMan->SetUserInitialization(new ImpActionInitialization(detCon));

    auto* visMan = new G4VisExecutive;
    visMan->Initialize();
    auto* uiMan = G4UImanager::GetUIpointer();

    // G4OpticalParameters::Instance()->SetProcessActivation("Scintillation", false);

    uiMan->ApplyCommand("/control/macroPath macros");
    bool interactive = (argc == 1);
    if (interactive) {
        auto* uiExec = new G4UIExecutive(argc, argv);
        uiMan->ApplyCommand("/control/execute macros/init_gui.mac");
        uiExec->SessionStart();
        delete uiExec;
    }
    else {
        uiMan->ApplyCommand("/run/initialize");
        std::stringstream ss;
        ss << "/control/execute " << argv[1];
        uiMan->ApplyCommand(ss.str());
    }

    delete runMan;
    delete visMan;
    return 0;
}
