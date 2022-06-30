#include "impress.hh"

void initRandom()
{
    auto x = std::random_device()();
    G4cout << "random device gave " << x << G4endl;
    G4Random::setTheSeed(x);
}

int main(int argc, char* argv[])
{
    initRandom();
    // load stuff
    const auto& igf = ImpGlobalConfigs::instance();

    bool doScintillate = igf.configOption<bool>(ImpGlobalConfigs::kENABLE_SCINTILLATION);
    bool onlyDetector = igf.configOption<bool>(ImpGlobalConfigs::kBUILD_ONLY_DETECTOR);
    G4String flareSize = igf.configOption<std::string>(ImpGlobalConfigs::kATTENUATOR_CHOICE);
    ImpAnalysis::instance()->updateFlareIdentifier(flareSize);

    auto* runMan = new G4MTRunManager;
    const char* envNumCores = std::getenv("SLURM_NUM_CORES");
    size_t numCores;
    if (envNumCores != nullptr) {
        std::stringstream ss(envNumCores);
        ss >> numCores;
    }
    else {
        numCores = G4Threading::G4GetNumberOfCores();
    }
    runMan->SetNumberOfThreads(numCores);

    auto* physList = new ImpPhysicsList;
    physList->SetVerboseLevel(0);
    runMan->SetUserInitialization(physList);

    ImpVDetectorConstruction* detCon;
    if (onlyDetector) detCon = new ImpOnlyDetectorConstruction(flareSize);
    else detCon = new ImpWholeSatelliteConstruction;

    runMan->SetUserInitialization(detCon);
    runMan->SetUserInitialization(new ImpActionInitialization);

    auto* uiMan = G4UImanager::GetUIpointer();

    uiMan->ApplyCommand("/process/had/verbose 0");
    G4OpticalParameters::Instance()->SetProcessActivation("Scintillation", doScintillate);
    G4OpticalParameters::Instance()->SetProcessActivation("Cerenkov", false);

    // silence annoying physics lists
    uiMan->ApplyCommand("/process/had/verbose 0");
    uiMan->ApplyCommand("/run/initialize");

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
        auto cmdStr = std::string("/control/execute ") + argv[1];
        uiMan->ApplyCommand(cmdStr);
    }

    delete runMan;
    return 0;
}
