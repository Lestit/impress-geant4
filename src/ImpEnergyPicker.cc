#include <iostream>
#include <fstream>

#include <G4Exception.hh>
#include <G4SystemOfUnits.hh>

#include <ImpEnergyPicker.hh>
#include <ImpEnergyPickerMessenger.hh>

namespace
{
    struct EngPointsFileData
    {
        std::vector<long double> eng;
        std::vector<long double> assoc;
    };

    void verifyIfs(const std::ifstream& ifs, const std::filesystem::path& p)
    {
        if (!ifs) {
            std::string excStr = "Cannot load element data file " + p.string();
            G4Exception(
                "src/ImpEnergyPicker.cc", "", RunMustBeAborted,
                excStr.c_str());
        }
    }

    EngPointsFileData loadGenericTable(const std::filesystem::path& p)
    {
        std::ifstream ifs(p);
        verifyIfs(ifs, p);

        long double energy, cdfVal;
        std::vector<long double> engVec, cdfVec;
        while (ifs >> energy >> cdfVal) {
            engVec.push_back(energy);
            cdfVec.push_back(cdfVal);
        }

        return {engVec, cdfVec};
    }

    EngPointsFileData loadFlareData(const std::string& flareSize)
    {
        std::string fn = "cdf_" + G4StrUtil::to_lower_copy(flareSize) + ".txt";
        std::filesystem::path fszPath(fn);
        return loadGenericTable(FLARE_CDFS_DIR / fszPath);
    }

    std::array<long double, NUM_O1_CDF>
    loadElementData(const std::string& elementName)
    {
        std::filesystem::path p(G4StrUtil::to_lower_copy(elementName) + "-o1.txt");
        std::ifstream ifs(ELT_INTENS_DIR / p);
        verifyIfs(ifs, p);

        long double e;
        size_t i = 0;
        std::array<long double, NUM_O1_CDF> ret;
        while (ifs >> e) {
            ret[i++] = e;
        }
        return ret;
    }
}

ImpEnergyPicker::ImpEnergyPicker() : 
    distrType(DistributionType::undefined),
    rng(std::random_device()()),
    realDis(0, 1),
    mess(new ImpEnergyPickerMessenger(this))
{ }

ImpEnergyPicker::ImpEnergyPicker(const ImpEnergyPicker& other)
        : distrType(other.distrType),
    monoEnergy(other.monoEnergy),
    flatStart(other.flatStart),
    flatEnd(other.flatEnd),
    flareEnergyVec(other.flareEnergyVec),
    flareEnergyCdf(other.flareEnergyCdf),
    eltEnergyO1Cdf(other.eltEnergyO1Cdf),
    rng(other.rng),
    realDis(other.realDis),
    mess(new ImpEnergyPickerMessenger(this))
{ }

ImpEnergyPicker& ImpEnergyPicker::operator=(const ImpEnergyPicker& other)
{
    if (this == &other) { return *this; }

    monoEnergy = other.monoEnergy;
    flatStart = other.flatStart;
    flatEnd = other.flatEnd;
    flareEnergyVec = other.flareEnergyVec;
    flareEnergyCdf = other.flareEnergyCdf;
    rng = other.rng;
    realDis = other.realDis;
    distrType = other.distrType;
    mess = new ImpEnergyPickerMessenger(this);
    return *this;
}

ImpEnergyPicker::~ImpEnergyPicker()
{
    if (mess) {
        delete mess;
        mess = nullptr;
    }
}

void ImpEnergyPicker::updateFlareSize(const std::string& fs)
{
    auto dat = loadFlareData(fs);
    flareEnergyVec = dat.eng;
    flareEnergyCdf = dat.assoc;
}

void ImpEnergyPicker::updateElement(const std::string& elt)
{
    eltEnergyO1Cdf = loadElementData(elt);
}

long double ImpEnergyPicker::pickEnergy()
{
    using dt = DistributionType;
    switch (distrType) {
        case dt::mono:    return pickMono(); break;
        case dt::flat:    return pickFlat(); break;
        case dt::flare:   return pickFlare(); break;
        case dt::element: return pickElement(); break;
        default:
            G4Exception(
                "ImpEnergyPicker::pickEnergy", "", RunMustBeAborted,
                "Undefined energy distribution type, or using GPS");
            return -0x1337;
    }
}

long double ImpEnergyPicker::pickElement()
{
    return eltEnergyO1Cdf[size_t(realDis(rng) * NUM_O1_CDF)] * keV;
}

long double ImpEnergyPicker::pickFlare()
{
    long double goal = realDis(rng);

    // max valid index
    std::size_t upperBoundIdx = flareEnergyCdf.size() - 1;
    for (std::size_t i = 0; i < flareEnergyCdf.size(); ++i) {
        if (flareEnergyCdf[i] >= goal) {
            upperBoundIdx = i;
            break;
        }
    }

    return interpolateFlareEnergy(upperBoundIdx, goal);
}

/* linear interpolation between energy/cdf points */
long double ImpEnergyPicker::interpolateFlareEnergy(std::size_t upperBound, long double goal)
{
    if (upperBound == 0) return flareEnergyVec[0];
   
    auto lowerBound = upperBound - 1;
    auto goalOffset = goal - flareEnergyCdf[lowerBound];
    auto deltaCdf = flareEnergyCdf[upperBound] - flareEnergyCdf[lowerBound];

    long double proportionBetween = goalOffset / deltaCdf;

    auto deltaEne = flareEnergyVec[upperBound] - flareEnergyVec[lowerBound];

    // put into Geant units
    return (flareEnergyVec[lowerBound] + proportionBetween * deltaEne) * keV;
}
