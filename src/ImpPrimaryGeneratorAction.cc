#include <G4Event.hh>
#include <G4ParticleTable.hh>
#include <G4SystemOfUnits.hh>
#include <G4UImanager.hh>

#include <ImpAnalysis.hh>
#include <ImpPrimaryGeneratorAction.hh>

ImpPrimaryGeneratorAction::ImpPrimaryGeneratorAction()
        : G4VUserPrimaryGeneratorAction(),
    gun(std::make_unique<G4ParticleGun>()),
    gps(std::make_unique<G4GeneralParticleSource>()),
    energyPicker(new ImpEnergyPicker),
    pointPicker(nullptr)
{
    auto* p = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
    gun->SetParticleDefinition(p);
    gun->SetParticleEnergy(50 * keV);

    G4ThreeVector minusZ(0, 0, -1);
    gun->SetParticleMomentum(minusZ);
}

void ImpPrimaryGeneratorAction::updateWorldVolume(const G4Box* box)
{ pointPicker = std::make_unique<ImpSurfacePointPicker>(box); }

void ImpPrimaryGeneratorAction::updateWorldVolume(const G4Tubs* cyl)
{ pointPicker = std::make_unique<ImpSurfacePointPicker>(cyl); }

// unique_ptrs auto delete
ImpPrimaryGeneratorAction::~ImpPrimaryGeneratorAction()
{
    if (energyPicker) {
        delete energyPicker;
        energyPicker = nullptr;
    }
}

void ImpPrimaryGeneratorAction::GeneratePrimaries(G4Event* evt)
{
    using dt = ImpEnergyPicker::DistributionType;
    long double e = 0;
    if (energyPicker->peekDistributionType() != dt::gps)
        e = energyPicker->pickEnergy();

    // allow G4GeneralParticleSource to bypass the ImpEnergyPicker
    if (energyPicker->peekDistributionType() == dt::gps) {
        gps->GeneratePrimaryVertex(evt);
        e = gps->GetParticleEnergy();
    }
    else if (pointPicker) {
        gun->SetParticlePosition(pointPicker->pickPoint());
        gun->SetParticleEnergy(e);
        gun->GeneratePrimaryVertex(evt);
    }
    else { gun->GeneratePrimaryVertex(evt); }

    ImpAnalysis::instance()->addIncidentEnergy(e);
}
