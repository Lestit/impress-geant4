#include <G4Box.hh>
#include <G4Threading.hh>
#include <G4Tubs.hh>
#include <G4UImanager.hh>

#include <ImpGpsConfig.hh>
#include <ImpOnlyDetectorConstruction.hh>
#include <ImpVDetectorConstruction.hh>
#include <ImpWholeSatelliteConstruction.hh>

#define DBG(x) {G4cout << (x) << G4endl; G4cout.flush();}

namespace {
    static const G4String CENTER_CMD = "/gps/pos/centre ";
    static const G4String HALFX_CMD = "/gps/pos/halfx ";
    static const G4String HALFY_CMD = "/gps/pos/halfy ";
    static const G4String RADIUS_CMD = "/gps/pos/radius ";

    static const G4String SHAPE_CMD = "/gps/pos/shape ";
    static const G4String POS_TYPE_CMD = "/gps/pos/type ";
    static const G4String ANG_TYPE_CMD = "/gps/ang/type ";
    static const G4String DIR_CMD = "/gps/direction ";

    static const G4String MIN_ENG_CMD = "/gps/ene/min ";
    static const G4String MAX_ENG_CMD = "/gps/ene/max ";
    static const G4String ENE_TYPE_CMD = "/gps/ene/type ";

    static const G4String HIST_TYPE_CMD = "/gps/hist/type ";
    static const G4String HIST_POINT_CMD = "/gps/hist/point ";
    static const G4String HIST_INTER_CMD = "/gps/hist/inter ";

    static const G4String PARTICLE_CMD = "/gps/particle ";
    static const G4String VERBOSE_CMD = "/gps/verbose ";
}

namespace ImpGpsConfig
{
    void configureGps(ImpVDetectorConstruction* dc, const G4String& flareSize)
    {
        using ct = ImpVDetectorConstruction::ConstructionType;
        if (dc->detectorConstructionType() == ct::onlyDetector)
            configureGpsCenter(static_cast<ImpOnlyDetectorConstruction*>(dc));
        else if (dc->detectorConstructionType() == ct::wholeSatellite)
            configureGpsCenter(static_cast<ImpWholeSatelliteConstruction*>(dc));

        configureCommonGpsParams(flareSize);
    }

    void configureGpsCenter(ImpOnlyDetectorConstruction* dc)
    {
        G4cout << "=== START CONFIGURING GPS ===" << G4endl;
        std::stringstream ss;
        auto* ui = G4UImanager::GetUIpointer();
        auto* worldCyl = dc->peekBoundingCyl();

        ui->ApplyCommand(SHAPE_CMD + "Circle");
        ss << RADIUS_CMD << worldCyl->GetOuterRadius()/cm << " cm";
        ui->ApplyCommand(ss.str());
        
        ss.str("");
        ss << CENTER_CMD << "0 0 " << worldCyl->GetZHalfLength()/cm << " cm";
        ui->ApplyCommand(ss.str());
        G4cout << "=== DONE CONFIGURING GPS ===" << G4endl;
    }

    void configureGpsCenter(ImpWholeSatelliteConstruction* dc)
    {
        std::stringstream ss;
        auto* ui = G4UImanager::GetUIpointer();
        auto* worldBox = dc->peekBoundingBox();

        ui->ApplyCommand(SHAPE_CMD + "Square");
        ss << HALFX_CMD << worldBox->GetXHalfLength()/cm << " cm";
        ui->ApplyCommand(ss.str());

        ss.str("");
        ss << HALFY_CMD << worldBox->GetYHalfLength()/cm << " cm";
        ui->ApplyCommand(ss.str());

        ss.str("");
        ss << CENTER_CMD << "0 0 " << worldBox->GetZHalfLength()/cm << " cm";
        ui->ApplyCommand(ss.str());
    }

    void configureCommonGpsParams(const G4String& flareSize)
    {
        // THIS IS SO FRAGILE!!!!!!!!
        auto* uiMan = G4UImanager::GetUIpointer();
        uiMan->ApplyCommand(VERBOSE_CMD + "0");
        uiMan->ApplyCommand(POS_TYPE_CMD + "Plane");
        uiMan->ApplyCommand(ANG_TYPE_CMD + "planar");
        uiMan->ApplyCommand(PARTICLE_CMD + "gamma");
        uiMan->ApplyCommand(DIR_CMD + "0 0 -1");

        const G4String fn = dataFileFrom(flareSize);
        std::ifstream ifs(fn);
        if (!ifs) {
            throw std::runtime_error("Couldn't open file " + fn + ": " + strerror(errno));
        }

        double minE, maxE;
        ifs >> minE >> maxE;

        std::stringstream ss;
        ss << MIN_ENG_CMD << minE << " MeV";
        uiMan->ApplyCommand(ss.str());

        ss.str("");
        ss << MAX_ENG_CMD << maxE << " MeV";
        uiMan->ApplyCommand(ss.str());
        
        uiMan->ApplyCommand(ENE_TYPE_CMD + "Arb");
        uiMan->ApplyCommand(HIST_TYPE_CMD + "arb");

        G4double curE, intens;
        while (ifs >> curE >> intens) {
            ss.str("");
            ss << HIST_POINT_CMD << curE << " " << intens;
            uiMan->ApplyCommand(ss.str());
        }
        uiMan->ApplyCommand(HIST_INTER_CMD + "Lin");
    }

    G4String dataFileFrom(G4String flareSize)
    {
        flareSize.toLower();
        static const G4String FLARE_DATA_DIR = "flare-data";
        return FLARE_DATA_DIR + "/gps_" + flareSize + ".txt";
    }
}
