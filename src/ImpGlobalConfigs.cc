#include <ImpGlobalConfigs.hh>

#include <G4Exception.hh>

#include <fstream>
#include <stdexcept>

const std::string ImpGlobalConfigs::CFG_FN = "impress.conf";

const std::unordered_map<std::string, G4double> ImpGlobalConfigs::ATTENUATOR_THICKNESSES = {
    {"c1",   00.000 * um},
    {"m1",   60.000 * um},
    {"m5",   210.00 * um},
    {"x1",   340.00 * um},
    {"test", 5.0000 * mm}
};

// static specified in .hh
const std::string ImpGlobalConfigs::kSAVE_SI_POSITIONS            = "save-si-positions";
const std::string ImpGlobalConfigs::kSAVE_SI_ENERGIES             = "save-si-energies";
const std::string ImpGlobalConfigs::kAIR_GAP_THICKNESS            = "air-gap-thickness";
const std::string ImpGlobalConfigs::kCEBR3_LENGTH                 = "cebr3-length";
const std::string ImpGlobalConfigs::kCEBR3_EDGE_FINISH            = "cebr3-edge-finish";
const std::string ImpGlobalConfigs::kCEBR3_BACK_FINISH            = "cebr3-back-finish";
const std::string ImpGlobalConfigs::kOPTICAL_DRAW_FREQUENCY       = "optical-photon-draw-freq";
const std::string ImpGlobalConfigs::kLIGHT_GUIDE_THICKNESS        = "light-guide-thickness";
const std::string ImpGlobalConfigs::kBUILD_LIGHT_GUIDE_REFLECTOR  = "build-light-guide-reflector";
const std::string ImpGlobalConfigs::kSI_SPACING                   = "si-spacing";
const std::string ImpGlobalConfigs::kSAVE_PREFIX                  = "data-folder-prefix";
const std::string ImpGlobalConfigs::kSCINT_TIME_CONST             = "scint-time-const";
const std::string ImpGlobalConfigs::kSAVE_CRYST_POSITIONS         = "save-cryst-positions";
const std::string ImpGlobalConfigs::kMAKE_ONLY_CRYSTAL            = "make-only-crystal";
const std::string ImpGlobalConfigs::kNUM_SIPMS                    = "num-sipms";
const std::string ImpGlobalConfigs::kSAVE_EACH_CRYST_HIT_ENERGY   = "save-each-crystal-hit-energy";
const std::string ImpGlobalConfigs::kREFLECTOR_SHIM_SIDE_LENGTH   = "reflector-shim-side-length";
const std::string ImpGlobalConfigs::kBUILD_TEFLON_BETWEEN_SIPMS   = "build-teflon-between-sipms";

namespace { using pt = ImpGlobalConfigs::ParseType; }
const std::unordered_map<std::string, pt> ImpGlobalConfigs::KEY_TYPE_PARSE = {
    {kSAVE_SI_POSITIONS,            pt::vBOOL},
    {kSAVE_SI_ENERGIES,             pt::vBOOL},
    {kAIR_GAP_THICKNESS,            pt::vDOUBLE},
    {kCEBR3_LENGTH,                 pt::vDOUBLE},
    {kCEBR3_EDGE_FINISH,            pt::vSTRING},
    {kCEBR3_BACK_FINISH,            pt::vSTRING},
    {kOPTICAL_DRAW_FREQUENCY,       pt::vINT},
    {kLIGHT_GUIDE_THICKNESS,        pt::vDOUBLE},
    {kBUILD_LIGHT_GUIDE_REFLECTOR,  pt::vBOOL},
    {kSI_SPACING,                   pt::vDOUBLE},
    {kSAVE_PREFIX,                  pt::vSTRING},
    {kSCINT_TIME_CONST,             pt::vDOUBLE},
    {kSAVE_CRYST_POSITIONS,         pt::vBOOL},
    {kMAKE_ONLY_CRYSTAL,            pt::vBOOL},
    {kNUM_SIPMS,                    pt::vINT},
    {kSAVE_EACH_CRYST_HIT_ENERGY,   pt::vBOOL},
    {kREFLECTOR_SHIM_SIDE_LENGTH,   pt::vDOUBLE},
    {kBUILD_TEFLON_BETWEEN_SIPMS,   pt::vBOOL}
};

const ImpGlobalConfigs& ImpGlobalConfigs::instance()
{
    static ImpGlobalConfigs igc;
    return igc;
}

ImpGlobalConfigs::ImpGlobalConfigs()
{
    loadConfig();    
}

ImpGlobalConfigs::~ImpGlobalConfigs()
{ }

void ImpGlobalConfigs::loadConfig()
{
    std::ifstream ifs(CFG_FN);
    if (!ifs) {
        throw std::runtime_error(
            "can't load " + CFG_FN + "! make sure it's in the executible directory.");
    }

    std::string key;
    using pt = ImpGlobalConfigs::ParseType;
    while (ifs >> key) {
        try {
            const auto& loadType = KEY_TYPE_PARSE.at(key);
            switch (loadType) {
                case pt::vBOOL: {
                    bool b;
                    ifs >> b;
                    configMap[key] = b;
                    break;
                } case pt::vDOUBLE: {
                    double d;
                    ifs >> d;
                    configMap[key] = d;
                    break;
                } case pt::vINT: {
                    int i;
                    ifs >> i;
                    configMap[key] = i;
                    break;
                } case pt::vSTRING: {
                    std::string st;
                    ifs >> st;
                    configMap[key] = st;
                    break;
                } default: {
                    throw std::out_of_range("got a value type that can't be parsed in ImpGlobalConfigs");
                    break;
                }
            }
        }
        catch (const std::out_of_range& e) {
            auto errStr = std::string(e.what()) + " -> key was " + key;
            G4Exception(
                "ImpGlobalConfigs/loadConfig", "lcfg0", RunMustBeAborted,
                errStr.c_str());
        }
    }
}
