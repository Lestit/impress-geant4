#pragma once

#include <G4SystemOfUnits.hh>
#include <G4String.hh>

#include <map>

namespace ImpGlobalConf {
    // from analytical model
    static const std::map<G4String, G4double> ATTENUATOR_THICKNESSES = {
        {"c1", 00.00000000 * um},
        {"m1", 23.43750000 * um},
        {"m5", 156.2500000 * um},
        {"x1", 265.6250000 * um},
        {"test", 5 * mm}
    };

    static const G4double HACKY_DETECTOR_EFF = 1.0;
}
