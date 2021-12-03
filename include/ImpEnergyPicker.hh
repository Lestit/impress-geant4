#pragma once

#include <array>
#include <filesystem>
#include <memory>
#include <random>
#include <vector>
#include <string>

#include <G4String.hh>
#include <G4SystemOfUnits.hh>
#include <G4Types.hh>

class ImpEnergyPickerMessenger;

static const std::filesystem::path FLARE_CDFS_DIR = "flare-cdfs";
static const std::filesystem::path ELT_INTENS_DIR = "element-intens";
static const size_t NUM_O1_CDF = 30000;

class ImpEnergyPicker
{
    public:
        enum class DistributionType {
            flare, mono, flat, element,
            gps, undefined };

        ImpEnergyPicker(const ImpEnergyPicker& other);
        ImpEnergyPicker& operator=(const ImpEnergyPicker& other);

        ImpEnergyPicker();
        // flare
        /* ImpEnergyPicker(const std::string& flareSize); */
        // monoenergetic
        /* ImpEnergyPicker(long double mono); */
        // flat distribution
        /* ImpEnergyPicker(long double flatStart, long double flatEnd); */
        ~ImpEnergyPicker();

        void updateDistributionType(const DistributionType& dt)
        { distrType = dt; }
        const DistributionType& peekDistributionType()
        { return distrType; }

        void updateFlareSize(const std::string& fs);
        void updateElement(const std::string& elt);
        void updateMonoEnergy(long double mono)
        { monoEnergy = mono; }
        void updateFlatEnergyBounds(long double start, long double end)
        { flatStart = start; flatEnd = end; }

        long double pickEnergy();
    private:
        long double pickFlare();
        long double pickElement();
        long double pickFlat()
        {
            auto diff = flatEnd - flatStart;
            auto rand = realDis(rng);
            auto stride = diff * rand;
            return flatStart + stride;
        }

        long double pickMono()
        { return monoEnergy; }

        long double interpolateFlareEnergy(std::size_t upperBound, long double goal);

        DistributionType distrType;
        long double monoEnergy;

        long double flatStart;
        long double flatEnd;

        std::vector<long double> flareEnergyVec;
        std::vector<long double> flareEnergyCdf;

        std::array<long double, NUM_O1_CDF> eltEnergyO1Cdf;

        std::mt19937_64 rng;
        std::uniform_real_distribution<long double> realDis;

        ImpEnergyPickerMessenger* mess;
};
