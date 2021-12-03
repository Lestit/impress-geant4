#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4SystemOfUnits.hh>

#include <ImpEnergyPickerMessenger.hh>

#define SAFE_DELETE(x) if(x) { delete x; x = nullptr; }

std::unordered_map<ImpEnergyPickerMessenger::dtype, G4String>
ImpEnergyPickerMessenger::distrBackwards = {
    {ImpEnergyPickerMessenger::dtype::mono, "mono"},
    {ImpEnergyPickerMessenger::dtype::flat, "flat"},
    {ImpEnergyPickerMessenger::dtype::flare, "flare"},
    {ImpEnergyPickerMessenger::dtype::gps, "gps"},
    {ImpEnergyPickerMessenger::dtype::element, "element"}
};

ImpEnergyPickerMessenger::ImpEnergyPickerMessenger(ImpEnergyPicker* ePicker)
        : ePicker(ePicker),
    impDir(new G4UIdirectory(IMP_DIR_STR)),
    flareEnergyCmd(new G4UIcmdWithAString(FLARE_ENE_STR, this)),
    elementEnergyCmd(new G4UIcmdWithAString(ELT_ENE_STR, this)),
    monoEnergyCmd(new G4UIcmdWithADoubleAndUnit(MONO_ENE_STR, this)),
    flatEnergyLowCmd(new G4UIcmdWithADoubleAndUnit(FLAT_ENE_LOW_STR, this)),
    flatEnergyHighCmd(new G4UIcmdWithADoubleAndUnit(FLAT_ENE_UPP_STR,this)),
    distributionTypeCmd(new G4UIcmdWithAString(DISTR_TYPE_STR, this))
{
    impDir->SetGuidance("IMPRESS energy commands");

    flareEnergyCmd->SetParameterName(FLARE_ENE_PRM, false);
    flareEnergyCmd->SetDefaultValue("x1");
    flareEnergyCmd->SetCandidates(ALLOWED_FLARES);

    elementEnergyCmd->SetParameterName(ELT_ENE_PRM, false);
    elementEnergyCmd->SetDefaultValue("ba133");
    elementEnergyCmd->SetCandidates(ALLOWED_ELEMENTS);

    distributionTypeCmd->SetParameterName(DISTR_TYPE_PARM, false);
    distributionTypeCmd->SetDefaultValue("mono");
    distributionTypeCmd->SetCandidates(ALLOWED_DISTRIBUTIONS);

    monoEnergyCmd->SetParameterName(MONO_ENE_PARM, false);
    monoEnergyCmd->SetDefaultValue(50 * keV);
    monoEnergyCmd->SetUnitCategory("Energy");

    flatEnergyLowCmd->SetParameterName(FLAT_ENE_LOW_PARM, false);
    flatEnergyLowCmd->SetDefaultValue(1 * keV);
    flatEnergyLowCmd->SetUnitCategory("Energy");
    
    flatEnergyHighCmd->SetParameterName(FLAT_ENE_HIGH_PARM, false);
    flatEnergyHighCmd->SetDefaultValue(100 * keV);
    flatEnergyHighCmd->SetUnitCategory("Energy");
}

ImpEnergyPickerMessenger::~ImpEnergyPickerMessenger()
{
    SAFE_DELETE(impDir);
    SAFE_DELETE(flareEnergyCmd);
    SAFE_DELETE(elementEnergyCmd);
    SAFE_DELETE(monoEnergyCmd);
    SAFE_DELETE(flatEnergyLowCmd);
    SAFE_DELETE(flatEnergyHighCmd);
    SAFE_DELETE(distributionTypeCmd);
}

void ImpEnergyPickerMessenger::SetNewValue(
        G4UIcommand* cmd, G4String newValues)
{
    if (cmd == distributionTypeCmd) {
        distrType = newValues == "mono" ? dtype::mono       : 
                    newValues == "flat" ? dtype::flat       :
                    newValues == "gps"  ? dtype::gps        :
                    newValues == "element" ? dtype::element : dtype::flare;
        ePicker->updateDistributionType(distrType);
    }
    else if (cmd == flareEnergyCmd)
        processFlare(newValues);
    else if (cmd == elementEnergyCmd)
        processElement(newValues);
    else if (cmd == flatEnergyLowCmd)
        processFlatLow(newValues);
    else if (cmd == flatEnergyHighCmd)
        processFlatHigh(newValues);
    else if (cmd == monoEnergyCmd)
        processMono(newValues);
}

G4String ImpEnergyPickerMessenger::GetCurrentValue(G4UIcommand *cmd)
{
    if (cmd == flareEnergyCmd)
        return flareSize;
    else if (cmd == elementEnergyCmd)
        return isotopeName;
    else if (cmd == monoEnergyCmd)
        return monoEnergyCmd->ConvertToString(G4double(mono));
    else if (cmd == flatEnergyLowCmd)
        return flatEnergyLowCmd->ConvertToString(G4double(flatLow));
    else if (cmd == flatEnergyHighCmd)
        return flatEnergyHighCmd->ConvertToString(G4double(flatHigh));
    else if (cmd == distributionTypeCmd)
        return distrBackwards[distrType];

    return "";
}

void ImpEnergyPickerMessenger::processFlare(const G4String& nv)
{
    checkType(dtype::flare, "Not using flare distribution");
    flareSize = nv;
    ePicker->updateFlareSize(flareSize);
}

void ImpEnergyPickerMessenger::processElement(const G4String& nv)
{
    checkType(dtype::element, "Not using element distribution");
    isotopeName = nv;
    ePicker->updateElement(isotopeName);
}

void ImpEnergyPickerMessenger::processFlatLow(const G4String& nv)
{
    checkType(dtype::flat, "Not using flat distribution");
    flatLow = flatEnergyLowCmd->GetNewDoubleValue(nv);
    ePicker->updateFlatEnergyBounds(flatLow, flatHigh);
}

void ImpEnergyPickerMessenger::processFlatHigh(const G4String& nv)
{
    checkType(dtype::flat, "Not using flat distribution");
    flatHigh = flatEnergyHighCmd->GetNewDoubleValue(nv);
    ePicker->updateFlatEnergyBounds(flatLow, flatHigh);
}

void ImpEnergyPickerMessenger::processMono(const G4String& nv)
{
    checkType(dtype::mono, "Not using a monoenergetic distribution");
    mono = monoEnergyCmd->GetNewDoubleValue(nv);
    ePicker->updateMonoEnergy(mono);
}

void ImpEnergyPickerMessenger::checkType(dtype t, const char* errorMsg)
{
    if (t != distrType)
        G4Exception("ImpEnergyPickerMessenger::checkType", "", RunMustBeAborted, errorMsg);
}
