#pragma once
#include <G4SystemOfUnits.hh>

namespace {
    static constexpr G4double inch = 25.4 * mm;

    static const G4String CYL = "_cyl",
                          PHY = "_phy",
                          LOG = "_log",
                          SUR = "_sur",
                          EDGE = "_edge",
                          BACK = "_back";

    static const G4String CHANNEL_PFX = "hafx_channel";

    static const G4String CHANNEL_LOG_PFX = "hafx_logical";
    static const G4String CHANNEL_CYL_PFX = "hafx_bounding_cyl";
    static const G4String WHOLE_CHANNEL_LOG_VOL_PFX = "hafx_log_vol";

    static const G4String CRYSTAL_CYL_PFX = "hafx_crystal_cyl";
    static const G4String CRYSTAL_LOG_PFX = "hafx_crystal_log";
    static const G4String CRYSTAL_PHY_PFX = "hafx_crystal_phy";
    static const G4String CRYSTAL_SUR_PFX = "hafx_crystal_sur";

    static const G4String BETWEEN_TEFLON_CRYSTAL = "hafx_between";

    static const G4String TEFLON_RING = "hafx_teflon_ring";
    static const G4String TEFLON_CAP = "hafx_teflon_cap";
    static const G4String TEFLON_SURF = "hafx_teflon_surf";
    /* static const G4String TEFLON_SHAPE_PFX = "hafx_teflon_union"; */
    /* static const G4String TEFLON_LOG_PFX = "hafx_teflon_log"; */
    /* static const G4String TEFLON_PHY_PFX = "hafx_teflon_phy"; */

    static const G4String AL_CYL_PFX = "hafx_al_cyl";
    static const G4String AL_LOG_PFX = "hafx_al_log";
    static const G4String AL_PHY_PFX = "hafx_al_phy";
    static const G4String AL_SURF_PFX = "hafx_al_surf";

    static const G4String ATT_CYL_PFX = "hafx_att_cyl";
    static const G4String ATT_LOG_PFX = "hafx_att_log";
    static const G4String ATT_PHY_PFX = "hafx_att_phy";
    static const G4String ATT_SUR_PFX = "hafx_att_sur";

    static const G4String QZ_CYL_PFX = "hafx_qz_cyl";
    static const G4String QZ_LOG_PFX = "hafx_qz_log";
    static const G4String QZ_PHY_PFX = "hafx_qz_phy";
    static const G4String QZ_SUR_PFX = "hafx_qz_sur";
    static const G4String QZ_CRYST_INTERFACE_PFX = "hafx_qz_cebr3_interface";

    static const G4String BE_CYL_PFX = "hafx_be_cyl";
    static const G4String BE_LOG_PFX = "hafx_be_log";
    static const G4String BE_PHY_PFX = "hafx_be_phy";
    static const G4String BE_SUR_PFX = "hafx_be_sur";

    static const G4String CRYSTAL_SENS_DET_PFX = "hafx_crystal_sd";
    static const G4String SI_SENS_DET_PFX = "hafx_dumb_sd";

    static const G4String LG_BOX_PFX = "hafx_light_guide_box";
    static const G4String LG_LOG_PFX = "hafx_light_guide_log";
    static const G4String LG_PHY_PFX = "hafx_light_guide_phy";
    static const G4String LG_WRAP_BOX_PFX = "hafx_light_guide_wrap_box";
    static const G4String LG_WRAP_SOL_PFX = "hafx_light_guide_wrap_sol";
    static const G4String LG_WRAP_LOG_PFX = "hafx_light_guide_wrap_log";
    static const G4String LG_WRAP_PHY_PFX = "hafx_light_guide_wrap_phy";

    static const G4String SI_BOX_PFX = "hafx_si_box";
    static const G4String SI_LOG_PFX = "hafx_si_log";
    static const G4String SI_PHY_PFX = "hafx_si_phy";
    static const G4String SI_SUR_PFX = "hafx_si_sur";

    static const G4String PAINT_CYL_PFX = "hafx_paint_cyl";
    static const G4String PAINT_SOL_PFX = "hafx_paint_sol";
    static const G4String PAINT_LOG_PFX = "hafx_paint_log";
    static const G4String PAINT_PHY_PFX = "hafx_paint_phy";
    static const G4String PAINT_SUR_PFX = "hafx_paint_sur";
    
    static const G4String SIPM_SHIM_BOX_PFX = "hafx_sipm_shim_box";
    static const G4String SIPM_SHIM_LOG_PFX = "hafx_sipm_shim_log";
    static const G4String SIPM_SHIM_PHY_PFX = "hafx_sipm_shim_phy";

    // numerics
    static constexpr G4double WHOLE_DIAMETER = 43 * mm;

    static constexpr G4double BE_THICKNESS = 0.7 * mm;

    static constexpr G4double CEBR3_DIAMETER = 37 * mm;

    static constexpr G4double TEFLON_THICKNESS = 0.127 * mm;

    static constexpr G4double AL_HOUSING_THICKNESS = 1.25 * mm;
    static constexpr G4double AL_DEPTH_DELTA = 6.8 * mm;

    // ???
    static constexpr G4double QUARTZ_THICKNESS = 3 * mm;
    static constexpr G4double QUARTZ_DIAMETER = 40.5 * mm;

    // guess for now
    static constexpr G4double LIGHT_GUIDE_SIDE_LENGTH = 1 * inch;
    static constexpr G4double SI_THICKNESS = 0.3 * mm;
    static constexpr G4double BROADCOM_FULL_LENGTH = 3.88 * mm;

    static constexpr G4double SIPM_REFLECTOR_SHIM_DEPTH = 1 * um;
}
