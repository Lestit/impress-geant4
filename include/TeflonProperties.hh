#pragma once

namespace ImpMaterials {
    static const G4String kNIST_TEFLON = "G4_TEFLON";
    
    // to be better filled in once precise teflon type is known
    static const std::vector<G4double> TEFLON_REFR_IDX_ENERGIES = {
        2.5*eV, 10*eV
    };

    static const std::vector<G4double> TEFLON_REFR_IDXS = {
        1.38, 1.38
    };
}
