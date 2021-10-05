#pragma once

#include <G4SystemOfUnits.hh>
#include <G4String.hh>

namespace ImpGlobalConf {
    // from analytical model
    static const std::map<G4String, G4double> ATTENUATOR_THICKNESSES = {
        {"c1", 00.00000000 * um},
        {"m1", 21.48437500 * um},
        {"m5", 156.2500000 * um},
        {"x1", 257.8125000 * um},
        {"test", 5 * mm},
    };
}
