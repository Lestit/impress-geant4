#include "impress.hh"

int main(int argc, char* argv[])
{
    // random engine
    // no customization for now?
    char c = 0;
    do {
        std::cout << "Scintillation on? (y/n) ";
        std::cin >> c;
    } while (c != 'y' && c != 'n');
    bool doScintillate = (c == 'y');

    c = 0;
    do {
        std::cout << "Whole (s)atellite or just a single (d)etector? (s/d) ";
        std::cin >> c;
    } while (c != 's' && c != 'd');
    bool wholeSatellite = (c == 's');

    G4String flareSize;
    static const std::array<G4String, 4> fs = {"c1", "m1", "m5", "x1"};
    do {
        std::cout << "Flare size to simulate (c1, m1, m5, x1) ";
        std::cin >> flareSize;
    } while (std::find(fs.begin(), fs.end(), flareSize) == fs.end());

    auto* runMan = new G4MTRunManager;
    // assumes hyperthreading (2 threads/core)
    runMan->SetNumberOfThreads(2 * G4Threading::G4GetNumberOfCores());

    auto* physList = new ImpPhysicsList;
    physList->SetVerboseLevel(0);
    runMan->SetUserInitialization(physList);

    ImpVDetectorConstruction* detCon;
    if (wholeSatellite) detCon = new ImpWholeSatelliteConstruction;
    else detCon = new ImpOnlyDetectorConstruction(flareSize);

    runMan->SetUserInitialization(detCon);
    runMan->SetUserInitialization(new ImpActionInitialization(detCon, flareSize));

    auto* uiMan = G4UImanager::GetUIpointer();

    G4OpticalParameters::Instance()->SetProcessActivation("Scintillation", doScintillate);

    uiMan->ApplyCommand("/run/initialize");
    // ImpGpsConfig::configureGps(detCon, flareSize);

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
