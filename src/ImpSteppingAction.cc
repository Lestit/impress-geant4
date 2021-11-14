#include <G4OpBoundaryProcess.hh>
#include <G4OpticalPhoton.hh>
#include <G4ProcessManager.hh>
#include <G4Step.hh>
#include <G4Threading.hh>

#include <ImpSteppingAction.hh>
#include <ChannelConstants.hh>

#include <map>

namespace {
    G4Mutex optMux = G4MUTEX_INITIALIZER;
    std::map<size_t, const char*> optProcLookup = {
        {0, "Undefined"},
        {1, "Transmission"},
        {2, "FresnelRefraction"},
        {3, "FresnelReflection"},
        {4, "TotalInternalReflection"},
        {5, "LambertianReflection"},
        {6, "LobeReflection"},
        {7, "SpikeReflection"},
        {8, "BackScattering"},
        {9, "Absorption"},
        {10, "Detection"},
        {11, "NotAtBoundary"},
        {12, "SameMaterial"},
        {13, "StepTooSmall"},
        {14, "NoRINDEX"},
        {15, "PolishedLumirrorAirReflection"},
        {16, "PolishedLumirrorGlueReflection"},
        {17, "PolishedAirReflection"},
        {18, "PolishedTeflonAirReflection"},
        {19, "PolishedTiOAirReflection"},
        {20, "PolishedTyvekAirReflection"},
        {21, "PolishedVM2000AirReflection"},
        {22, "PolishedVM2000GlueReflection"},
        {23, "EtchedLumirrorAirReflection"},
        {24, "EtchedLumirrorGlueReflection"},
        {25, "EtchedAirReflection"},
        {26, "EtchedTeflonAirReflection"},
        {27, "EtchedTiOAirReflection"},
        {28, "EtchedTyvekAirReflection"},
        {29, "EtchedVM2000AirReflection"},
        {30, "EtchedVM2000GlueReflection"},
        {31, "GroundLumirrorAirReflection"},
        {32, "GroundLumirrorGlueReflection"},
        {33, "GroundAirReflection"},
        {34, "GroundTeflonAirReflection"},
        {35, "GroundTiOAirReflection"},
        {36, "GroundTyvekAirReflection"},
        {37, "GroundVM2000AirReflection"},
        {38, "GroundVM2000GlueReflection"},
        {39, "Dichroic"},
    };
}

ImpSteppingAction::ImpSteppingAction() :
    G4UserSteppingAction()
{ }

ImpSteppingAction::~ImpSteppingAction()
{ }

void ImpSteppingAction::UserSteppingAction(const G4Step* step)
{
    auto* track = step->GetTrack();
    if (track->GetDefinition() == G4OpticalPhoton::Definition()) {
        processOptical(step);
    }
}

void ImpSteppingAction::processOptical(const G4Step* step)
{
    return;
    // G4AutoLock l(&optMux);
    static G4ThreadLocal G4OpBoundaryProcess* boundary = nullptr;
    if (boundary == nullptr) {
        // find the optical boundary
        auto* pv = step->
            GetTrack()->
            GetDefinition()->
            GetProcessManager()->
            GetProcessList();
        size_t cnt = 0;
        for (size_t i = 0; i < pv->size(); ++i) {
            if ((*pv)[i]->GetProcessName() == "OpBoundary") {
                boundary = static_cast<G4OpBoundaryProcess*>((*pv)[i]);
                cnt++;
            }
        }
        G4cout << "number of boundaries " << cnt << G4endl;
    }

    if (boundary == nullptr) {
        G4cout << "******** issue finding optical boundary" << G4endl;
        return;
    }

    const auto* prePt = step->GetPreStepPoint();
    const auto* postPt = step->GetPostStepPoint();

    const auto* preVol = prePt? prePt->GetPhysicalVolume() : nullptr;
    const auto* postVol = postPt? postPt->GetPhysicalVolume() : nullptr;

    const auto preName = preVol? preVol->GetName() : "";
    const auto postName = postVol? postVol->GetName() : "";
    bool yesSilicon = postName.contains(SI_PHY_PFX) || preName.contains(SI_PHY_PFX);

    if (postPt->GetStepStatus() == fGeomBoundary) {
        // G4cout << "TRACKING OPTICAL AFTER BOUNDARY" << G4endl;
        auto s = boundary->GetStatus();
        switch (s) {
            case Detection:
                G4cout << "optical photon DETECTED!!!!!!!!!!!!!!!!!!!!" << G4endl;
                break;
            case Absorption:
            case TotalInternalReflection:
            case StepTooSmall:
            case Transmission:
            case FresnelRefraction:
            case FresnelReflection:
                break;
            default:
                if (yesSilicon) {
                    G4cout << "something weird: " << optProcLookup[s] << G4endl
                           << "in volume " << step->GetTrack()->GetVolume()->GetName() << G4endl
                           << "pre volume " << preName << G4endl
                           << "post volume " << postName << G4endl;
                }
                break;
        }
    }
}
