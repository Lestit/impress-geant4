#include "ImpMaterials.hh"

#include <ImpGlobalConfigs.hh>

// forward declare here; don't put in header
namespace ImpMaterials {
    void configureTeflon();
    void makeCeBr3();
    void configureCeBr3Scintillation();
    void makeVacuum();
    void makeAluminum();
    void configureQuartz();
    void makeBeryllium();
    void makeHousingAluminumAlloy();
    void makeSilicon();
    void makePdms();
}

namespace {
    static const G4bool useSpline = true;
}

namespace ImpMaterials
{
    void makeMaterials()
    {
        if (!G4Material::GetMaterial(kVACUUM))
            makeVacuum();

        if (!G4Material::GetMaterial(kCEBR3)) {
            makeCeBr3();
            configureCeBr3Scintillation();
        }

        auto* nMan = G4NistManager::Instance();
        if (!nMan->FindOrBuildMaterial(kNIST_SIO2)->GetMaterialPropertiesTable())
            configureQuartz();
        if (!nMan->FindOrBuildMaterial(kNIST_TEFLON)->GetMaterialPropertiesTable())
            configureTeflon();
        if (!nMan->FindOrBuildMaterial(kNIST_SI)->GetMaterialPropertiesTable())
            makeSilicon();

        if (!G4Material::GetMaterial(kAL))
            makeAluminum();
        if (!G4Material::GetMaterial(kBE))
            makeBeryllium();
        if (!G4Material::GetMaterial(kHOUSING_ALLOY))
            makeHousingAluminumAlloy();
        if (!G4Material::GetMaterial(kPDMS))
            makePdms();
    }

    void configureTeflon()
    {
        auto* nistMan = G4NistManager::Instance();
        auto* teflon = nistMan->FindOrBuildMaterial(kNIST_TEFLON);
        auto* tefPt = new G4MaterialPropertiesTable;

        tefPt->AddProperty(kREFR_IDX, TEFLON_REFR_IDX_ENERGIES, TEFLON_REFR_IDXS, useSpline);
        tefPt->AddProperty(kREFLECTIVITY, TEFLON_REFR_IDX_ENERGIES, TEFLON_REFLECTIVITY, useSpline);

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
        // Removed in G4 v11 - ws March 2022
        /* opp->SetScintEnhancedTimeConstants(false); */

        auto* cebr3 = G4Material::GetMaterial(kCEBR3);
        if (!cebr3) {
            G4Exception(
                "src/ImpMaterials.cc configureScintillation()",
                "[no error code]",
                RunMustBeAborted,
                "Cerium bromide not loaded before configuring scintillation");
        }

        auto* scintPt = new G4MaterialPropertiesTable;

        scintPt->AddConstProperty(kSCINT_YIELD, CEBR3_SCINT_YIELD);
        scintPt->AddProperty(kREFR_IDX, CEBR3_REFR_IDX_ENERGIES, CEBR3_REFR_IDXS, useSpline);

        // updated in v11
        scintPt->AddProperty(
            kONLY_SCINT_COMPONENT, CEBR3_SCINT_OPTICAL_ENERGIES,
            CEBR3_SCINT_OPTICAL_INTENSITIES, useSpline);
        scintPt->AddConstProperty(
            kONLY_TIME_CONSTANT,
            ImpGlobalConfigs::instance().configOption<double>(ImpGlobalConfigs::kSCINT_TIME_CONST) * ns);

        // # of photons emitted = RESOLUTION_SCALE * sqrt(mean # of photons)
        // TODO: modify source to make energy resolution energy-dependent
        scintPt->AddConstProperty(kRESOLUTION_SCALE, CEBR3_SCINT_RESLN_SCALE);
        scintPt->AddProperty(kABSORPTION_LEN, CEBR3_ABS_LEN_ENERGIES, CEBR3_ABS_LEN, useSpline);
        // skip optical Rayleigh scattering (not important)
        // skip Mie scattering (doesn't apply)

        cebr3->SetMaterialPropertiesTable(scintPt);
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

        std::vector<G4double> indices(CEBR3_REFR_IDX_ENERGIES.size(), 1.);
        vacPt->AddProperty(kREFR_IDX, CEBR3_REFR_IDX_ENERGIES, indices, useSpline);
        /* vacPt->AddProperty(kREFR_IDX_REAL, CEBR3_REFR_IDX_ENERGIES, indices, useSpline); */
        /* vacPt->AddProperty(kREFR_IDX_IMAG, CEBR3_REFR_IDX_ENERGIES, std::vector<G4double>(indices.size(), 0.), useSpline); */

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
            SATELLITE_TEMP,
            VACUUM_PRESSURE);

        al->AddElement(alElt, G4double(1.0));
        auto* alPt = new G4MaterialPropertiesTable;

        // DO NOT ADD JUST RINDEX!!!!!!!!!!!!!!!!!!!1
        /* alPt->AddProperty(kREFR_IDX, AL_REFR_IDX_ENERGIES, AL_REFR_IDX_REAL); */
        alPt->AddProperty(kREFR_IDX_REAL, AL_REFR_IDX_ENERGIES, AL_REFR_IDX_REAL);
        alPt->AddProperty(kREFR_IDX_IMAG, AL_REFR_IDX_ENERGIES, AL_REFR_IDX_IMAG);
        al->SetMaterialPropertiesTable(alPt);
    }

    void configureQuartz()
    {
        auto* qz = G4NistManager::Instance()->FindOrBuildMaterial(kNIST_SIO2);
        auto* qzPt = new G4MaterialPropertiesTable;
        qzPt->AddProperty("RINDEX", QZ_REFR_IDX_ENERGIES, QZ_REFR_IDXS);
        /* qzPt->AddProperty(kREFR_IDX_REAL, QZ_REFR_IDX_ENERGIES, QZ_REFR_IDXS); */
        /* qzPt->AddProperty(kREFR_IDX_IMAG, QZ_REFR_IDX_ENERGIES, std::vector<G4double>(QZ_REFR_IDXS.size(), 0.0)); */

        qz->SetMaterialPropertiesTable(qzPt);
    }

    void makeBeryllium()
    {
        auto* beElt = G4NistManager::Instance()->FindOrBuildElement("Be");
        
        auto* be = new G4Material(
            kBE,
            BE_DENSITY,
            BE_NUM_COMPONENTS,
            kStateSolid,
            SATELLITE_TEMP);
        be->AddElement(beElt, G4int(1));

        auto* bePt = new G4MaterialPropertiesTable;
        bePt->AddProperty(kREFR_IDX, BE_REFR_IDX_ENERGIES, BE_REFR_IDX_REAL);
        bePt->AddProperty(kREFR_IDX_REAL, BE_REFR_IDX_ENERGIES, BE_REFR_IDX_REAL);
        bePt->AddProperty(kREFR_IDX_IMAG, BE_REFR_IDX_ENERGIES, BE_REFR_IDX_IMAG);
        be->SetMaterialPropertiesTable(bePt);
    }

    void makeHousingAluminumAlloy()
    {
        // from ASTM international doi:10.1520/B0209M-14
        // average values for spec
        static const std::map<G4String, G4double> eltMap = {
            {"Al", 0.9748},
            {"Mg", 0.01},
            {"Si", 0.006},
            {"Fe", 0.0035},
            {"Cr", 0.00195},
            {"Zn", 0.00125},
            {"Tl", 0.00125},
            {"Mn", 0.00125}
        };

        // the alloy density is the same to 3 significant figures
        static const G4double alloyDensity = AL_DENSITY;
        const auto alloyNumComp = eltMap.size();

        auto* housingAlloy = new G4Material(
            kHOUSING_ALLOY, 
            alloyDensity,
            G4int(alloyNumComp),
            kStateSolid,
            SATELLITE_TEMP,
            VACUUM_PRESSURE);

        auto* nistMan = G4NistManager::Instance();
        for (const auto& eltPair : eltMap) {
            auto* e = nistMan->FindOrBuildElement(eltPair.first);
            housingAlloy->AddElement(e, eltPair.second);
        }

        // optically, the alloy is very nearly aluminum.
        housingAlloy->SetMaterialPropertiesTable(
            G4Material::GetMaterial(kAL)->
            GetMaterialPropertiesTable());
    }

    void makeSilicon()
    {
        auto* si = G4NistManager::Instance()->FindOrBuildMaterial(kNIST_SI);

        auto* simpt = new G4MaterialPropertiesTable;
        std::vector<G4double> refl(SI_BROADCOM_NUMBERS.size(), 0.);

        // set to one and apply detection in post-processing (?)
        simpt->AddProperty(kOP_DET_EFF, SI_DET_EFF_ENERGIES, SI_DET_EFF, useSpline);
        simpt->AddProperty(kREFLECTIVITY, SI_DET_EFF_ENERGIES, refl, useSpline);
        simpt->AddProperty(kTRANSMITTANCE, SI_DET_EFF_ENERGIES, SI_TRANSMITTANCE, useSpline);
        simpt->AddProperty(kREFR_IDX, SI_REFR_IDX_ENERGY, SI_REFR_IDX_REAL, useSpline);
        simpt->AddProperty(kREFR_IDX_REAL, SI_REFR_IDX_ENERGY, SI_REFR_IDX_REAL, useSpline);
        simpt->AddProperty(kREFR_IDX_IMAG, SI_REFR_IDX_ENERGY, SI_REFR_IDX_IMAG, useSpline);

        si->SetMaterialPropertiesTable(simpt);
    }

    void makePdms()
    {
        auto* nm = G4NistManager::Instance();
        auto* hydrogen = nm->FindOrBuildMaterial("G4_H");
        auto* carbon = nm->FindOrBuildMaterial("G4_C");
        auto* oxygen = nm->FindOrBuildMaterial("G4_O");
        auto* silicon = nm->FindOrBuildMaterial("G4_Si");

        std::array<G4Material*, 4> elMats = {hydrogen, carbon, oxygen, silicon};
        // for computing mass fraction later
        double totalMass = 0;
        std::array<std::uint8_t, 4> atomz = {6, 2, 1, 1};
        for (std::size_t i = 0; i < atomz.size(); ++i) {
            totalMass += elMats[i]->GetMassOfMolecule() * atomz[i];
        }

        auto* pdms = new G4Material(
            kPDMS, PDMS_DENSITY, G4int(atomz.size()),
            kStateSolid, SATELLITE_TEMP, VACUUM_PRESSURE);

        for (std::size_t i = 0; i < atomz.size(); ++i) {
            double massFrac = elMats[i]->GetMassOfMolecule() / totalMass;
            pdms->AddMaterial(elMats[i], atomz[i] * massFrac);
        }

        auto* pdmsPt = new G4MaterialPropertiesTable;
        pdmsPt->AddProperty(kREFR_IDX, PDMS_REFR_IDX_ENERGIES, PDMS_REFR_IDXS, useSpline);
        pdms->SetMaterialPropertiesTable(pdmsPt);
    }
}
