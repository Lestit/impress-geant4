#include "ImpPrimaryGeneratorAction.hh"

ImpPrimaryGeneratorAction::ImpPrimaryGeneratorAction()
{
    // one particle at a time
    gun = std::make_shared<G4ParticleGun>(1);
    auto* table = G4ParticleTable::GetParticleTable();
    gun->SetParticleDefinition(
        table->FindParticle("gamma"));

    gun->SetParticleEnergy(10 * keV);
    gun->SetParticlePosition(G4ThreeVector(0, 0, 10 * cm));
    gun->SetParticleMomentumDirection(G4ThreeVector(0, 0, -1));
}

// shared_ptr auto deletes
ImpPrimaryGeneratorAction::~ImpPrimaryGeneratorAction()
{ }

void ImpPrimaryGeneratorAction::GeneratePrimaries(G4Event* evt)
{ gun->GeneratePrimaryVertex(evt); }
