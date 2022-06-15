#include "impress.hh"
/* #define RUN_UNTIL_DIE */

void initRandom()
{
    auto x = std::random_device()();
    G4cout << "random device gave " << x << G4endl;
    G4Random::setTheSeed(x);
}

G4String pickFlareSize()
{
    std::vector<G4String> fs;
    for (const auto& p : ImpGlobalConfigs::ATTENUATOR_THICKNESSES)
        fs.push_back(p.first);

    std::stringstream flaresStream;
    for (const auto& fn : fs)
        flaresStream << fn << ", ";

    G4String flareSize;
    do {
        std::cout << "Flare size to simulate -- also for attenuator thickness ("
                  << flaresStream.str() << ") ";
        std::cin >> flareSize;
    } while (std::find(fs.begin(), fs.end(), flareSize) == fs.end());

    ImpAnalysis::instance()->updateFlareIdentifier(flareSize);
    return flareSize;
}

int main(int argc, char* argv[])
{
    initRandom();
    // load stuff
    (void) ImpGlobalConfigs::instance();

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
    if (!wholeSatellite) {
        flareSize = pickFlareSize();
    }

    auto* runMan = new G4MTRunManager;
    runMan->SetNumberOfThreads(G4Threading::G4GetNumberOfCores());

    auto* physList = new ImpPhysicsList;
    physList->SetVerboseLevel(0);
    runMan->SetUserInitialization(physList);

    ImpVDetectorConstruction* detCon;
    if (wholeSatellite) detCon = new ImpWholeSatelliteConstruction;
    else detCon = new ImpOnlyDetectorConstruction(flareSize);

    runMan->SetUserInitialization(detCon);
    runMan->SetUserInitialization(new ImpActionInitialization(detCon));

    auto* uiMan = G4UImanager::GetUIpointer();

    uiMan->ApplyCommand("/process/had/verbose 0");
    G4OpticalParameters::Instance()->SetProcessActivation("Scintillation", doScintillate);
    G4OpticalParameters::Instance()->SetProcessActivation("Cerenkov", false);

    // silence annoying physics lists
    uiMan->ApplyCommand("/process/had/verbose 0");
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
#ifdef RUN_UNTIL_DIE
        while (true) {
            G4cout << "RUN AGAIN" << G4endl;
            uiMan->ApplyCommand("/run/beamOn 10000");
        }
#endif
    }

    delete runMan;
    return 0;
}
