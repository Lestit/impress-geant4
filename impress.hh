#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>

#include <ImpAnalysis.hh>
#include <ImpGlobalConfigs.hh>
#include <ImpActionInitialization.hh>
#include <ImpWholeSatelliteConstruction.hh>
#include <ImpOnlyDetectorConstruction.hh>
#include <ImpPhysicsList.hh>

#include <G4Box.hh>
#include <G4UIExecutive.hh>
#include <G4VisExecutive.hh>
#include <G4UIManager.hh>
#include <G4OpticalParameters.hh>
#include <G4RunManagerFactory.hh>
#include <G4String.hh>
#include <G4Threading.hh>
