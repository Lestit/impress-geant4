#include <algorithm>
#include <map>

#include <G4EventManager.hh>
#include <G4OpBoundaryProcess.hh>
#include <G4OpticalPhoton.hh>
#include <G4ProcessManager.hh>
#include <G4SDManager.hh>
#include <G4Step.hh>
#include <G4Threading.hh>
#include <Randomize.hh>

#include <ImpGlobalConfigs.hh>
#include <ImpEventAction.hh>
#include <ImpHafxChannel.hh>
#include <ImpSiSensitiveDetector.hh>
#include <ImpSteppingAction.hh>
#include <ChannelConstants.hh>

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

    G4OpBoundaryProcess* findOpticalboundary(const G4Step* step)
    {
        G4OpBoundaryProcess* boundary = nullptr;
        auto* pv = step->
            GetTrack()->
            GetDefinition()->
            GetProcessManager()->
            GetProcessList();
        size_t cnt = 0;
        for (size_t i = 0; i < pv->size(); ++i) {
            if ((*pv)[i]->GetProcessName() == "OpBoundary") {
                boundary = static_cast<G4OpBoundaryProcess*>((*pv)[i]);
                if (cnt++ > 0) {
                    G4Exception(
                        "ImpSteppingAction/findOpticalBoundary",
                        "fob0", RunMustBeAborted, "More than one optical boundary present. Uh oh.");
                }
            }
        }

        if (boundary == nullptr) {
            G4cout << "******** issue finding optical boundary" << G4endl;
        }

        return boundary; 
    }
}

ImpSteppingAction::ImpSteppingAction() :
    G4UserSteppingAction()
{ }

ImpSteppingAction::~ImpSteppingAction()
{ }

void ImpSteppingAction::UserSteppingAction(const G4Step* step)
{
    trackScintillation(step);
    auto* track = step->GetTrack();
    if (track->GetDefinition() == G4OpticalPhoton::Definition()) {
        processOptical(step);
    }
}

void ImpSteppingAction::trackScintillation(const G4Step* step)
{
    const std::vector<const G4Track*>* secs;
    if ((secs = step->GetSecondaryInCurrentStep()) && secs->size() > 0) {
        size_t numOps = std::count_if(
            secs->begin(), secs->end(), [](const G4Track* t) {
            return t->GetParticleDefinition()->GetParticleName() == "opticalphoton"; });
        // store the number of optical photons generated
        // to be saved at end of event
        auto* ea = dynamic_cast<ImpEventAction*>(
            G4EventManager::GetEventManager()->GetUserEventAction());
        if (ea) ea->addScintPhot(numOps);
    }
}

void ImpSteppingAction::processOptical(const G4Step* step)
{
    /* return; */
    // G4AutoLock l(&optMux);
    static G4ThreadLocal G4OpBoundaryProcess* boundary = nullptr;
    if (boundary == nullptr) {
        boundary = findOpticalboundary(step);
    }

    const auto* prePt = step->GetPreStepPoint();
    const auto* postPt = step->GetPostStepPoint();

    const auto* preVol = prePt? prePt->GetPhysicalVolume() : nullptr;
    const auto* postVol = postPt? postPt->GetPhysicalVolume() : nullptr;

    const auto preName = preVol? preVol->GetName() : "";
    const auto postName = postVol? postVol->GetName() : "";
    bool yesSilicon = postName.contains(SI_PHY_PFX) || preName.contains(SI_PHY_PFX);

    if (postPt->GetStepStatus() == fGeomBoundary) {
        auto stat = boundary->GetStatus();
        switch (stat) {
            // detect it here bc it dies before it can actually register inside the detector
            case Detection:
                processDetected(preVol, postVol, step);
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
                    const auto n = std::string(
                            step? step->GetTrack()? step->GetTrack()->GetVolume()?
                            step->GetTrack()->GetVolume()->GetName() : "" : "" : "");
                    G4cout << "something weird: " << optProcLookup[stat] << G4endl
                           << "in volume " << n << G4endl
                           << "pre volume " << preName << G4endl
                           << "post volume " << postName << G4endl;
                }
                break;
        }
    }
}

void ImpSteppingAction::processDetected(
    const G4VPhysicalVolume* preVol, const G4VPhysicalVolume* postVol, const G4Step* step)
{
    // allow for ad-hoc efficiency
    if (G4UniformRand() > ImpGlobalConf::HACKY_DETECTOR_EFF) return;

    std::array<const G4VPhysicalVolume*, 2> volz = {preVol, postVol};
    for (const auto* v : volz) {
        const auto* sdLogVol = v->GetLogicalVolume();
        if (!sdLogVol) continue;

        auto* siSd = dynamic_cast<ImpSiSensitiveDetector*>(sdLogVol->GetSensitiveDetector());
        if (siSd) siSd->processOptical(step);
    }
}
