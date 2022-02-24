//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file OptDebugSteppingAction.cc
/// \brief Implementation of the OptDebugSteppingAction class

#include "OptDebugSteppingAction.hh"
#include "OptDebugEventAction.hh"
#include "OptDebugDetectorConstruction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4OpBoundaryProcess.hh"

namespace {
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

    G4OpBoundaryProcess* findOpticalBoundary(const G4Step* step)
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
                        "OptDebugSteppingAction/findOpticalBoundary",
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

OptDebugSteppingAction::OptDebugSteppingAction(OptDebugEventAction* eventAction)
: G4UserSteppingAction()
{}

OptDebugSteppingAction::~OptDebugSteppingAction()
{}

void OptDebugSteppingAction::UserSteppingAction(const G4Step* step)
{
    const auto* prePt = step->GetPreStepPoint();
    const auto* postPt = step->GetPostStepPoint();

    const auto* preVol = prePt? prePt->GetPhysicalVolume() : nullptr;
    const auto* postVol = postPt? postPt->GetPhysicalVolume() : nullptr;

    const auto preName = preVol? preVol->GetName() : "";
    const auto postName = postVol? postVol->GetName() : "";

    if (postPt->GetStepStatus() == fGeomBoundary) {
        auto* boundary = findOpticalBoundary(step);
        if (boundary == nullptr) { return; }
        auto stat = boundary->GetStatus();
        switch (stat) {
            // detect it here bc it dies before it can actually register inside the detector
            case NoRINDEX:
                G4cout << "**** NoRINDEX: " << G4endl
                       << "**** pre vol" << preName << G4endl
                       << "**** post vol" << postName << G4endl;
                break;
            case Absorption:
                G4cout << "**** ABSORBED ****" << G4endl
                       << "**** pre vol " << preName << G4endl
                       << "**** post vol " << postName << G4endl;
            case Detection:
            case TotalInternalReflection:
            case StepTooSmall:
            case Transmission:
            case FresnelRefraction:
            case FresnelReflection:
            case LambertianReflection:
            case SpikeReflection:
                break;
            default:
                const auto n = std::string(
                    step? step->GetTrack()? step->GetTrack()->GetVolume()?
                    step->GetTrack()->GetVolume()->GetName() : "" : "" : "");
                G4cout << "something weird: " << optProcLookup[stat] << G4endl
                       << "in volume " << n << G4endl
                       << "pre volume " << preName << G4endl
                       << "post volume " << postName << G4endl;
                break;
        }
    }
}
