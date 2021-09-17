#include <iostream>
#include <fstream>

#include <G4Exception.hh>
#include <G4SystemOfUnits.hh>

#include <ImpEnergyPicker.hh>

namespace {
    struct FileData {
        std::vector<long double> eng;
        std::vector<long double> cdf;
    };
    FileData loadFlareData(const std::string& flareSize)
    {
        G4String cpy = flareSize;
        cpy.toLower();
        std::string fn = "cdf_" + cpy + ".txt";
        std::filesystem::path fszPath(fn);
        auto p = FLARE_CDFS_DIR / fszPath;
        std::ifstream ifs(p);

        if (!ifs) {
            std::string excStr = "Cannot load flare data file " + p.string();
            G4Exception(
                "src/ImpEnergyPicker.cc", "", RunMustBeAborted,
                excStr.c_str());
        }

        long double energy, cdfVal;
        std::vector<long double> engVec, cdfVec;
        while (ifs >> energy >> cdfVal) {
            engVec.push_back(energy);
            cdfVec.push_back(cdfVal);
        }

        return {engVec, cdfVec};
    }
}

ImpEnergyPicker::ImpEnergyPicker(
    const std::vector<long double>& energies, const std::vector<long double>& energyCdf)
        : energyVec(energies),
    energyCdf(energyCdf),
    rng(std::random_device()()),
    realDis(0, 1)
{ }

ImpEnergyPicker::~ImpEnergyPicker()
{ }

long double ImpEnergyPicker::pickEnergy()
{
    long double goal = realDis(rng);

    // max valid index
    std::size_t upperBoundIdx = energyCdf.size() - 1;
    for (std::size_t i = 0; i < energyCdf.size(); ++i) {
        if (energyCdf[i] >= goal) {
            upperBoundIdx = i;
            break;
        }
    }

    return interpolateEnergy(upperBoundIdx, goal);
}

/* linear interpolation between energy/cdf points */
long double ImpEnergyPicker::interpolateEnergy(std::size_t upperBound, long double goal)
{
    if (upperBound == 0) return energyVec[0];
   
    auto lowerBound = upperBound - 1;
    auto goalOffset = goal - energyCdf[lowerBound];
    auto deltaCdf = energyCdf[upperBound] - energyCdf[lowerBound];

    long double proportionBetween = goalOffset / deltaCdf;

    auto deltaEne = energyVec[upperBound] - energyVec[lowerBound];

    // put into Geant units
    return (energyVec[lowerBound] + proportionBetween * deltaEne) * keV;
}

ImpEnergyPicker ImpEnergyPicker::fromFlareSize(const std::string& flareSize)
{
    auto dat = loadFlareData(flareSize);
    return ImpEnergyPicker(dat.eng, dat.cdf);
}

std::unique_ptr<ImpEnergyPicker> ImpEnergyPicker::uniquePtrFromFlareSize(const std::string& flareSize)
{
    auto dat = loadFlareData(flareSize);
    return std::make_unique<ImpEnergyPicker>(dat.eng, dat.cdf);
}

/* FileData loadFlareData(const std::string& flareSize) */
