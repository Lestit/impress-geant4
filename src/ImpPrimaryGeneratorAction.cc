#include <G4Event.hh>
#include <G4ParticleTable.hh>
#include <G4SystemOfUnits.hh>

#include <ImpPrimaryGeneratorAction.hh>
ImpPrimaryGeneratorAction::ImpPrimaryGeneratorAction()
        : G4VUserPrimaryGeneratorAction(),
    gun(std::make_unique<G4ParticleGun>()),
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
    if (energyPicker && pointPicker) {
        gun->SetParticlePosition(pointPicker->pickPoint());
        gun->SetParticleEnergy(energyPicker->pickEnergy());
    }

    gun->GeneratePrimaryVertex(evt);
}
