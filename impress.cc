#include "impress.hh"

void configureGps(G4UImanager* uiMan, ImpDetectorConstruction* idc)
{
    // what a weird way to go about things
    uiMan->ApplyCommand("/control/execute macros/configure_gps.mac");
    const G4Box* b = idc->peekBoundingBox();

    std::stringstream ss;
    ss << "/gps/pos/centre " << "0 0"
       << " " <<  b->GetZHalfLength()/cm << " cm";
    uiMan->ApplyCommand(ss.str());
    ss.str("");

    ss << "/gps/pos/halfx " << b->GetXHalfLength()/cm << " cm";
    uiMan->ApplyCommand(ss.str());
    ss.str("");

    ss << "/gps/pos/halfy " << b->GetYHalfLength()/cm << " cm";
    uiMan->ApplyCommand(ss.str());
}

int main(int argc, char* argv[])
{
    // random engine
    // no customization for now?
    char c = 0;
    while (c != 'y' && c != 'n') {
        std::cout << "Scintillation on? (y/n) ";
        std::cin >> c;
    }
    bool doScintillate = (c == 'y');


    auto* runMan = new G4MTRunManager;
    // assumes hyperthreading (2 threads/core)
    runMan->SetNumberOfThreads(2 * G4Threading::G4GetNumberOfCores());

    auto* physList = new ImpPhysicsList;
    physList->SetVerboseLevel(0);
    runMan->SetUserInitialization(physList);
    auto* idc = new ImpDetectorConstruction;
    runMan->SetUserInitialization(idc);
    runMan->SetUserInitialization(new ImpActionInitialization);

    auto* uiMan = G4UImanager::GetUIpointer();

    G4OpticalParameters::Instance()->SetProcessActivation("Scintillation", doScintillate);
    uiMan->ApplyCommand("/run/initialize");
    configureGps(uiMan, idc);

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
        std::stringstream ss;
        ss << "/control/execute " << argv[1];
        uiMan->ApplyCommand(ss.str());
    }

    delete runMan;
    return 0;
}
