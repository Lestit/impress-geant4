#pragma once

#include <limits>
#include <cassert>

#include "G4Exception.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4NistManager.hh"
#include "G4String.hh"
#include "G4OpticalParameters.hh"
#include "G4PhysicalConstants.hh"
#include "G4Scintillation.hh"
#include "G4SystemOfUnits.hh"

#include "AluminumProperties.hh"
#include "BerylliumProperties.hh"
#include "CeBr3Properties.hh"
#include "HousingAlloyProperties.hh"
#include "QuartzProperties.hh"
#include "SiProperties.hh"
#include "TeflonProperties.hh"

/*
 * Empirical data sources
 * CeBr3 scintillation information
 *     Quarati et al doi:10.1016/j.nima.2013.08.005
 * CeBr3 index of refraction, absorption length info
 *     Li et al doi:10.1016/j.ssc.2007.08.040
 * Al refractive index info
 *     Rakic 1995 doi:10.1364/AO.34.004755
 */

// TODO: revisit the surrounding atmosphere of the satellite
namespace OptDebugMaterials {
    static const G4String kVACUUM = "vacuum";
    static const G4double VACUUM_MOLAR_DENSITY = 1.01 * g / mole;
    static const G4double VACUUM_ATOMIC_NUMBER = 1.0;
    static const G4double VACUUM_TEMPERATURE = 2.73 * kelvin;
    static const G4double VACUUM_PRESSURE = 1.322e-11 * pascal;

    static const G4double SATELLITE_TEMP = 283 * kelvin;
    void makeMaterials();
}
