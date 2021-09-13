#pragma once

#include <G4String.hh>

class ImpVDetectorConstruction;
class ImpOnlyDetectorConstruction;
class ImpWholeSatelliteConstruction;

namespace ImpGpsConfig {
    void configureGps(ImpVDetectorConstruction* dc, const G4String& flareSize);
    void configureGpsCenter(ImpOnlyDetectorConstruction* dc);
    void configureGpsCenter(ImpWholeSatelliteConstruction* dc);
    G4String dataFileFrom(G4String flareSize);
    void configureCommonGpsParams(const G4String& flareSize);
};
