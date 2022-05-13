#pragma once

#include <memory>
#include <unordered_map>

#include <G4UImessenger.hh>

#include <ImpEnergyPicker.hh>

class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;

class ImpEnergyPickerMessenger : public G4UImessenger
{
    public:
        ImpEnergyPickerMessenger(ImpEnergyPicker* ePicker);
        ~ImpEnergyPickerMessenger();

        void SetNewValue(G4UIcommand* command, G4String newValues) override;
        G4String GetCurrentValue(G4UIcommand* command) override;

    private:
        using dtype = ImpEnergyPicker::DistributionType;
        ImpEnergyPicker* ePicker;
        const char* IMP_DIR_STR = "/impe/";

        const char* MONO_ENE_STR = "/impe/mono";
        const char* MONO_ENE_PARM = "energy";

        const char* FLARE_ENE_STR = "/impe/flare";
        const char* FLARE_ENE_PRM = "flareSize";
        const char* ALLOWED_FLARES = "c1 c5 m1 m5 x1";

        const char* FLAT_ENE_LOW_STR = "/impe/flatLow";
        const char* FLAT_ENE_LOW_PARM = "flatLow";

        const char* FLAT_ENE_UPP_STR = "/impe/flatHigh";
        const char* FLAT_ENE_HIGH_PARM = "flatHigh";

        const char* DISTR_TYPE_STR = "/impe/distributionType";
        const char* DISTR_TYPE_PARM = "energyDistribution";
        const char* ALLOWED_DISTRIBUTIONS = "mono flat flare gps element";

        void processFlare(const G4String& nv);
        void processFlatLow(const G4String& nv);
        void processFlatHigh(const G4String& nv);
        void processMono(const G4String& nv);
        void checkType(dtype t, const char* errorMsg);

        G4UIdirectory* impDir;
        G4UIcmdWithAString* flareEnergyCmd;
        G4UIcmdWithADoubleAndUnit* monoEnergyCmd;
        G4UIcmdWithADoubleAndUnit* flatEnergyLowCmd;
        G4UIcmdWithADoubleAndUnit* flatEnergyHighCmd;
        G4UIcmdWithAString* distributionTypeCmd;

        long double flatLow;
        long double flatHigh;
        long double mono;
        G4String flareSize;
        G4String isotopeName;

        dtype distrType;
        static std::unordered_map<dtype, G4String> distrBackwards;
};
