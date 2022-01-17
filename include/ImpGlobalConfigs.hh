#pragma once

#include <G4SystemOfUnits.hh>
#include <G4String.hh>

#include <map>

namespace ImpGlobalConf {
    // from analytical model
    // round to nice numbers
    // last modified 24 dec 2021 (now using sunxspex + CHIANTI)
    static const std::map<G4String, G4double> ATTENUATOR_THICKNESSES = {
        {"c1", 00.000 * um},
        {"m1", 60.000 * um},
        {"m5", 210.00 * um},
        {"x1", 340.00 * um},
        {"test", 5 * mm}
    };

    static const G4double HACKY_DETECTOR_EFF = 1.0;
}
