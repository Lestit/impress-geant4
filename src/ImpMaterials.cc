#include "ImpMaterials.hh"

// forward declare here; don't put in header
namespace ImpMaterials {
    void configureTeflon();
    void makeCeBr3();
    void configureCeBr3Scintillation();
    void makeVacuum();
    void makeAluminum();
    void makeQuartz();
}

namespace ImpMaterials
{
    void makeMaterials()
    {
        if (!G4Material::GetMaterial(kVACUUM)) {
            makeVacuum();
        }

        if (!G4Material::GetMaterial(kCEBR3)) {
            makeCeBr3();
            configureCeBr3Scintillation();
        }

        configureTeflon();
        if (!G4Material::GetMaterial(kAL)) makeAluminum();
        if (!G4Material::GetMaterial(kNIST_SIO2)) makeQuartz();
    }

    void configureTeflon()
    {
        auto* nistMan = G4NistManager::Instance();
        auto* teflon = nistMan->FindOrBuildMaterial(kNIST_TEFLON);
        auto* tefPt = new G4MaterialPropertiesTable;

        tefPt->AddProperty(kREFR_IDX, TEFLON_REFR_IDX_ENERGIES, TEFLON_REFR_IDXS)->SetSpline(false);

        teflon->SetMaterialPropertiesTable(tefPt);
    }

    void makeCeBr3()
    {
        auto* nMan = G4NistManager::Instance();
        G4Element* ce = nMan->FindOrBuildElement("Ce");
        G4Element* br = nMan->FindOrBuildElement("Br");
        auto* cebr3 = new G4Material(
            kCEBR3,
            CEBR3_DENSITY,
            2.0,
            kStateSolid,
            SATELLITE_TEMP /* atmospheric pressure b/c hermetically sealed */);
        cebr3->AddElement(br, BR_MASS_FRAC);
        cebr3->AddElement(ce, CE_MASS_FRAC);
    }

    void configureCeBr3Scintillation() 
    {
        // to change optical parameters
        // see https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/TrackingAndPhysics/physicsProcess.html#id1
        auto* opp = G4OpticalParameters::Instance();
        if (!opp) {
            G4Exception(
                "src/ImpMaterials.cc configureScintillation()",
                "[no error code]",
                RunMustBeAborted,
                "Optical parameters table not instantiated");
        }
        opp->SetScintFiniteRiseTime(false);
        opp->SetScintEnhancedTimeConstants(false);
        // etc . . .

        auto* cebr3 = G4Material::GetMaterial(kCEBR3);
        if (!cebr3) {
            G4Exception(
                "src/ImpMaterials.cc configureScintillation()",
                "[no error code]",
                RunMustBeAborted,
                "Cerium bromide not loaded before configuring scintillation");
        }

        auto* scintPt = new G4MaterialPropertiesTable;

        G4bool useSpline = false;
        scintPt->AddConstProperty(kSCINT_YIELD, CEBR3_SCINT_YIELD);
        // refractive index depends on energy
        // only keep real part
        scintPt->AddProperty(kREFR_IDX, CEBR3_REFR_IDX_ENERGIES, CEBR3_REFR_IDXS)->SetSpline(useSpline);

        // we are only required to set one of either FASTCOMPONENT or SLOWCOMPONENT
        // optical photon relative intensitiesto get scintillation working.
        // for CeBr3 everything is fast anyway, so we just set the fast component.
        scintPt->AddProperty(kOPTICAL_FAST_COMP, CEBR3_SCINT_OPTICAL_ENERGIES, CEBR3_SCINT_OPTICAL_INTENSITIES)->SetSpline(useSpline);

        // similarly for the decay time constant.
        scintPt->AddConstProperty(kFAST_TIME_CONSTANT, CEBR3_DECAY_TIME_CONSTANT);

        // # of photons emitted = RESOLUTION_SCALE * sqrt(mean # of photons)
        // TODO: modify source to make energy resolution energy-dependent
        scintPt->AddConstProperty(kRESOLUTION_SCALE, CEBR3_SCINT_RESLN_SCALE);
        G4cout << "absorption length\n";
        G4cout.flush();
        scintPt->AddProperty(kABSORPTION_LEN, CEBR3_ABS_LEN_ENERGIES, CEBR3_ABS_LEN)->SetSpline(useSpline);
        // skip optical Rayleigh scattering (not important)
        // skip Mie scattering (doesn't apply)

        cebr3->SetMaterialPropertiesTable(scintPt);
        G4cout << "done\n";
        G4cout.flush();
    }

    void makeVacuum()
    {
        auto* vacMat = new G4Material(
            kVACUUM,
            VACUUM_ATOMIC_NUMBER,
            VACUUM_MOLAR_DENSITY, 
            universe_mean_density,
            kStateGas,
            VACUUM_TEMPERATURE,
            VACUUM_PRESSURE);

        auto* vacPt = new G4MaterialPropertiesTable;
        std::vector<G4double> indices = CEBR3_REFR_IDX_ENERGIES;
        std::fill(indices.begin(), indices.end(), 1.);
        vacPt->AddProperty(kREFR_IDX, CEBR3_REFR_IDX_ENERGIES, indices)->SetSpline(false);

        vacMat->SetMaterialPropertiesTable(vacPt);
    }

    void makeAluminum()
    {
        auto* nistMan = G4NistManager::Instance();
        auto* alElt = nistMan->FindOrBuildElement("Al");
        if (alElt == nullptr) G4cerr << "Nist manager gave nullptr for aluminum" << G4endl;

        auto* al = new G4Material(
            kAL,
            AL_DENSITY,
            AL_NUM_COMPONENTS,
            kStateSolid,
            SATELLITE_TEMP);

        al->AddElement(alElt, G4int(1));
        auto* alPt = new G4MaterialPropertiesTable;

        alPt->AddProperty(kREFR_IDX_REAL, AL_REFR_IDX_ENERGIES, AL_REFR_IDX_REAL);
        alPt->AddProperty(kREFR_IDX_REAL, AL_REFR_IDX_ENERGIES, AL_REFR_IDX_IMAG);
        al->SetMaterialPropertiesTable(alPt);
    }

    void makeQuartz()
    {
        auto* qz = G4NistManager::Instance()->FindOrBuildMaterial(kNIST_SIO2);
        auto* qzPt = new G4MaterialPropertiesTable;
        qzPt->AddProperty(kREFR_IDX, QZ_REFR_IDX_ENERGIES, QZ_REFR_IDXS);

        qz->SetMaterialPropertiesTable(qzPt);
    }
}
