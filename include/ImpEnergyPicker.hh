#pragma once

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

class ImpEnergyPicker
{
    public:
        enum class DistributionType { flare, mono, flat, undefined };

        ImpEnergyPicker(const ImpEnergyPicker& other);
        ImpEnergyPicker& operator=(const ImpEnergyPicker& other);

        ImpEnergyPicker();
        // flare
        ImpEnergyPicker(const std::string& flareSize);
        // monoenergetic
        ImpEnergyPicker(long double mono);
        // flat distribution
        ImpEnergyPicker(long double flatStart, long double flatEnd);
        ~ImpEnergyPicker();

        void updateDistributionType(const DistributionType& dt)
        { distrType = dt; }

        void updateFlareSize(const std::string& fs);
        void updateMonoEnergy(long double mono)
        { monoEnergy = mono; }
        void updateFlatEnergyBounds(long double start, long double end)
        { flatStart = start; flatEnd = end; }

        long double pickEnergy();
    private:
        long double pickFlare();
        long double pickFlat()
        {
            /* std::cout << "start is " << flatStart/keV << " keV" << std::endl */
            /*           << "end is   " << flatEnd/keV   << " keV" << std::endl */
            /*           << "diff is  " << diff/keV << " keV" << std::endl; */
            /* std::cout << "random num is " << rand << std::endl */
            /*           << "stride along diff is " << stride/keV << std::endl; */

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

        std::mt19937_64 rng;
        std::uniform_real_distribution<long double> realDis;

        ImpEnergyPickerMessenger* mess;
};