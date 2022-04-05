#include <ImpGlobalConfigs.hh>

#include <G4Exception.hh>

#include <fstream>
#include <stdexcept>

const std::string ImpGlobalConfigs::CFG_FN = "impress.conf";

const std::unordered_map<std::string, G4double> ImpGlobalConfigs::ATTENUATOR_THICKNESSES = {
    {"c1", 00.000 * um},
    {"m1", 60.000 * um},
    {"m5", 210.00 * um},
    {"x1", 340.00 * um},
    {"test", 5 * mm}
};

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

const std::unordered_map<std::string, ImpGlobalConfigs::ParseType> ImpGlobalConfigs::KEY_TYPE_PARSE = {
    {kSAVE_SI_POSITIONS,            ImpGlobalConfigs::ParseType::vBOOL},
    {kSAVE_SI_ENERGIES,             ImpGlobalConfigs::ParseType::vBOOL},
    {kAIR_GAP_THICKNESS,            ImpGlobalConfigs::ParseType::vDOUBLE},
    {kCEBR3_LENGTH,                 ImpGlobalConfigs::ParseType::vDOUBLE},
    {kCEBR3_EDGE_FINISH,            ImpGlobalConfigs::ParseType::vSTRING},
    {kCEBR3_BACK_FINISH,            ImpGlobalConfigs::ParseType::vSTRING},
    {kOPTICAL_DRAW_FREQUENCY,       ImpGlobalConfigs::ParseType::vINT},
    {kLIGHT_GUIDE_THICKNESS,        ImpGlobalConfigs::ParseType::vDOUBLE},
    {kBUILD_LIGHT_GUIDE_REFLECTOR, ImpGlobalConfigs::ParseType::vBOOL},
    {kSI_SPACING,                   ImpGlobalConfigs::ParseType::vDOUBLE},
    {kSAVE_PREFIX,                  ImpGlobalConfigs::ParseType::vSTRING}
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
