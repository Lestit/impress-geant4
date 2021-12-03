#include <algorithm>
#include <array>
#include <cassert>
#include <memory>
#include <fstream>
#include <sstream>

#include <ImpEnergyPicker.hh>
#include <G4SystemOfUnits.hh>

// new nov 2021
void testElement(ImpEnergyPicker& p, const std::string& eName);

void testUpdates();
void testMono(ImpEnergyPicker& p, long double monoEng);
void testFlat(ImpEnergyPicker& p, long double start, long double end);
void testAssignCopy();

void testAssignCopy()
{
    using dt = ImpEnergyPicker::DistributionType;
    ImpEnergyPicker pBase;
    pBase.updateMonoEnergy(30 * keV);
    pBase.updateFlatEnergyBounds(1 * keV, 300 * keV);
    pBase.updateFlareSize("x1");

    std::array<dt, 3> types = {dt::mono, dt::flare, dt::flat};
    for (const auto& t : types) {

        pBase.updateDistributionType(t);
        std::size_t nruns = 1000;
        for (std::size_t i = 0; i < nruns; ++i) {
            ImpEnergyPicker p1 = pBase;
            ImpEnergyPicker p2(pBase);
            ImpEnergyPicker p3(p2);
            ImpEnergyPicker p4(p1);

            std::array<ImpEnergyPicker, 4> pickerz = {p1, p2, p3, p4};
            std::array<long double, 4> energies;
            for (std::size_t j = 0; j < 4; ++j)
                energies[j] = pickerz[j].pickEnergy();

            if (!std::equal(energies.begin() + 1, energies.end(), energies.begin() + 1)) {
                for (const auto& e : energies) std::cout << e/keV << std::endl;
                throw std::runtime_error("copying didn't work right");
            }
        }
    }
}

void testUpdates()
{
    long double monoEng = 50 * keV;
    ImpEnergyPicker p;
    p.updateDistributionType(ImpEnergyPicker::DistributionType::mono);
    testMono(p, monoEng);

    p.updateDistributionType(ImpEnergyPicker::DistributionType::flare);
    p.updateFlareSize("x1");

    static const std::uint32_t ntrials = 1000000;
    for (std::uint32_t i = 0; i < ntrials; ++i) {
        auto e = p.pickEnergy();
        if (i % (ntrials / 10) == 0) {
            std::cout << "energy " << i << " is "
                      << e / keV << " keV" << std::endl;
        }
        if (e < 0) {
            throw std::runtime_error("negative energy in testUpdates");
        }
    }

   p.updateDistributionType(ImpEnergyPicker::DistributionType::flat);
   p.updateFlatEnergyBounds(1 * keV, 700 * keV);
   testFlat(p, 1 * keV, 700 * keV);

   p.updateDistributionType(ImpEnergyPicker::DistributionType::mono);
   p.updateMonoEnergy(monoEng);
   testMono(p, monoEng);
}

void testMono(ImpEnergyPicker& p, long double monoEng)
{
    for (size_t i = 0; i < 1000; ++i) {
        auto e = p.pickEnergy();
        if (e != monoEng) {
            std::ostringstream ss;
            ss << "monoenergetic error. expected " << monoEng / keV << " keV,"
                  " got " << e << " keV";
            throw std::runtime_error(ss.str());
        }
    }
}

void testFlat(ImpEnergyPicker& p, long double start, long double end)
{
    std::size_t nruns = 1000000;
    std::ofstream ofs("flat-test-out.csv");
    for (std::size_t i = 0; i < nruns; ++i) {
        auto e = p.pickEnergy();
        ofs << e/keV << std::endl;
        if (start > e || end < e) {
            std::ostringstream ss;
            ss << "flat error. got " << e / keV << "keV, which is out of bounds (index " << i << ")";
            throw std::runtime_error(ss.str());
        }
    }
}

void testElement(ImpEnergyPicker& p, const std::string& eltName)
{
    p.updateDistributionType(ImpEnergyPicker::DistributionType::element);
    p.updateElement(eltName);
    const std::size_t nruns = 30;
    for (std::size_t i = 0; i < nruns; ++i)
    {
        std::cout << std::setprecision(8) << p.pickEnergy() / keV << std::endl;
    }
}

int main()
{
    /* long double monoEng = 50 * keV; */
    auto ep = ImpEnergyPicker();
    /* ep.updateDistributionType(ImpEnergyPicker::DistributionType::mono); */
    /* ep.updateMonoEnergy(monoEng); */
    /* testMono(ep, monoEng); */
    /* long double start = 1. * keV, end = 300. * keV; */

    /* /1* ep = ImpEnergyPicker(start, end); *1/ */
    /* ep.updateDistributionType(ImpEnergyPicker::DistributionType::flat); */
    /* ep.updateFlatEnergyBounds(start, end); */
    /* testFlat(ep, start, end); */

    testElement(ep, "ba133");
    /* testUpdates(); */
    /* testAssignCopy(); */
    return 0;
}
