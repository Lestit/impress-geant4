#include <algorithm>
#include <iostream>
#include <fstream>
#include <memory>

#include <ImpEnergyPicker.hh>
#include <G4SystemOfUnits.hh>

const static size_t NUM_RUNS = 100000;

int main(int argc, char* argv[])
{
    std::string choice;
    if (argc == 1) choice = "x1";
    else { choice = argv[1]; }

    long double eTot = 0;
    long double eMin = 0;
    long double eMax = 0;
    auto ePicker = std::make_unique<ImpEnergyPicker>();
    ePicker->updateDistributionType(ImpEnergyPicker::DistributionType::flare);
    ePicker->updateFlareSize(choice);

    /* use helper-scripts/plot_recent_keep.py to plot this file for visual inspection */
    std::ofstream outf("recent_epick_test.txt");

    for (size_t i = 0; i < NUM_RUNS; ++i) {
        auto e = ePicker->pickEnergy();
        eTot += e;
        if (e < eMin) eMin = e;
        if (e > eMax) eMax = e;
        if (i % (NUM_RUNS / 10) == 0) std::cout << "run " << i << std::endl;

        outf << e / keV << std::endl;
    }

    std::cout << "Some statistics from " << NUM_RUNS << " runs:" << std::endl
              << "mean energy:         " << (eTot / NUM_RUNS / keV) << " keV" << std::endl
              << "max energy:          " << eMax / keV << " keV" << std::endl
              << "min energy:          " << eMin / keV << " keV" << std::endl;

    return 0;
}
