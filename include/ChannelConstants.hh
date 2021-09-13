#pragma once

namespace {
    /* G4Mutex teflonMux = G4MUTEX_INITIALIZER; */
    /* G4Mutex alMux = G4MUTEX_INITIALIZER; */
    /* G4Mutex beMux = G4MUTEX_INITIALIZER; */

    static const G4String CHANNEL_PFX = "hafx_channel";

    static const G4String CHANNEL_LOG_PFX = "hafx_logical";
    static const G4String CHANNEL_CYL_PFX = "hafx_bounding_cyl";
    static const G4String WHOLE_CHANNEL_LOG_VOL_PFX = "hafx_log_vol";

    static const G4String CRYSTAL_CYL_PFX = "hafx_crystal_cyl";
    static const G4String CRYSTAL_LOG_PFX = "hafx_crystal_log";
    static const G4String CRYSTAL_PHY_PFX = "hafx_crystal_phy";

    static const G4String TEFLON_RING = "hafx_teflon_ring";
    static const G4String TEFLON_CAP = "hafx_teflon_cap";
    static const G4String TEFLON_SHAPE_PFX = "hafx_teflon_union";
    static const G4String TEFLON_LOG_PFX = "hafx_teflon_log";
    static const G4String TEFLON_PHY_PFX = "hafx_teflon_phy";
    static const G4String TEFLON_SURF_PFX = "hafx_teflon_surf";
    static const G4String TEFLON_SKIN_LOG_PFX = "hafx_teflon_skin_log";

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
    static const G4String QZ_CRYST_INTERFACE_PFX = "hafx_qz_cebr3_interface";

    static const G4String BE_CYL_PFX = "hafx_be_cyl";
    static const G4String BE_LOG_PFX = "hafx_be_log";
    static const G4String BE_PHY_PFX = "hafx_be_phy";
    static const G4String BE_SUR_PFX = "hafx_be_sur";

    static const G4String SENSITIVE_DET_PFX = "hafx_crystal_sd";
}
