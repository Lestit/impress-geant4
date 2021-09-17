#pragma once

#include <filesystem>
#include <random>
#include <vector>
#include <string>

#include <G4String.hh>
#include <G4Types.hh>

static const std::filesystem::path FLARE_CDFS_DIR = "flare-cdfs";

class ImpEnergyPicker {
    public:
        static ImpEnergyPicker fromFlareSize(const std::string& flareSize);
        ImpEnergyPicker() =delete;
        ImpEnergyPicker(const std::vector<long double>& energies, const std::vector<long double>& energyCdf);
        ~ImpEnergyPicker();

        long double pickEnergy();

    private:
        long double interpolateEnergy(std::size_t upperBound, long double goal);

        const std::vector<long double> energyVec;
        const std::vector<long double> energyCdf;

        std::mt19937_64 rng;
        std::uniform_real_distribution<long double> realDis;
};

ImpEnergyPicker makeEnergyPicker(const std::string& flareSize);
