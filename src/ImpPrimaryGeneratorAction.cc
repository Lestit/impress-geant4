#include <G4Event.hh>
#include <G4ParticleTable.hh>
#include <G4SystemOfUnits.hh>

#include <ImpPrimaryGeneratorAction.hh>

ImpPrimaryGeneratorAction::ImpPrimaryGeneratorAction()
        : G4VUserPrimaryGeneratorAction(),
    gun(std::make_unique<G4ParticleGun>()),
    energyPicker(nullptr),
    pointPicker(nullptr)
{
    auto* p = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
    gun->SetParticleDefinition(p);
    gun->SetParticleEnergy(50 * keV);

    G4ThreeVector minusZ(0, 0, -1);
    gun->SetParticleMomentum(minusZ);
}

ImpPrimaryGeneratorAction::ImpPrimaryGeneratorAction(const G4Box* b, const G4String& flareSize)
        : ImpPrimaryGeneratorAction()
{
    energyPicker = ImpEnergyPicker::uniquePtrFromFlareSize(flareSize);
    pointPicker = std::make_unique<ImpSurfacePointPicker>(b);
}

ImpPrimaryGeneratorAction::ImpPrimaryGeneratorAction(const G4Tubs* c, const G4String& flareSize)
        : ImpPrimaryGeneratorAction()
{
    energyPicker = ImpEnergyPicker::uniquePtrFromFlareSize(flareSize);
    pointPicker = std::make_unique<ImpSurfacePointPicker>(c);
}

// unique_ptrs auto delete
ImpPrimaryGeneratorAction::~ImpPrimaryGeneratorAction()
{ }

void ImpPrimaryGeneratorAction::GeneratePrimaries(G4Event* evt)
{
    if (energyPicker && pointPicker) {
        gun->SetParticlePosition(pointPicker->pickPoint());
        gun->SetParticleEnergy(energyPicker->pickEnergy());
    }

    gun->GeneratePrimaryVertex(evt);
}
