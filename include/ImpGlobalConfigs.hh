#pragma once

#include <G4SystemOfUnits.hh>
#include <G4String.hh>

#include <any>
#include <unordered_map>
#include <string>

/******************************************************
 * To add a new config setting:
 *      - add a new key (like "kAIR_GAP_THICKNESS")
 *      - specify the conversion type in KEY_TYPE_PARSE
 ******************************************************/

class ImpGlobalConfigs
{
    public:
        static const ImpGlobalConfigs& instance();

        // delete these bc we have a singleton
        ImpGlobalConfigs(ImpGlobalConfigs&) =delete;
        void operator=(const ImpGlobalConfigs&) =delete;
        ~ImpGlobalConfigs();

        template<class T>
        T configOption(const std::string& key) const
        {
            try {
                return std::any_cast<T>(configMap.at(key));
            } catch (std::out_of_range& e) {
                std::cerr
                    << "key was: " << key << std::endl
                    << "emsg:    " << e.what() << std::endl;
                throw;
            }
        }
        
        static const std::string CFG_FN;

        // see .cc for what these are
        static const std::string
            kSAVE_SI_POSITIONS,
            kSAVE_SI_ENERGIES,
            kAIR_GAP_THICKNESS,
            kCEBR3_LENGTH,
            kCEBR3_EDGE_FINISH,
            kCEBR3_BACK_FINISH,
            kOPTICAL_DRAW_FREQUENCY,
            kLIGHT_GUIDE_THICKNESS,
            kBUILD_LIGHT_GUIDE_REFLECTOR,
            kSI_SPACING,
            kSAVE_PREFIX,
            kSCINT_TIME_CONST,
            kSAVE_CRYST_POSITIONS,
            kMAKE_ONLY_CRYSTAL;

        enum class ParseType { vBOOL, vDOUBLE, vINT, vSTRING };
        static const std::unordered_map<std::string, ParseType> KEY_TYPE_PARSE;

        // from analytical model
        // round to nice numbers
        // last modified 24 dec 2021 (now using sunxspex + CHIANTI)
        static const std::unordered_map<std::string, G4double> ATTENUATOR_THICKNESSES;
    private:
        ImpGlobalConfigs();
        void loadConfig();

        std::unordered_map<std::string, std::any> configMap;
};
