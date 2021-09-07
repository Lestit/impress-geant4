#include "G4GeneralParticleSource.hh"
#include "ImpPrimaryGeneratorAction.hh"

ImpPrimaryGeneratorAction::ImpPrimaryGeneratorAction() : 
    gps(std::make_unique<G4GeneralParticleSource>())
{
    // one particle at a time
    /* gun = std::make_shared<G4ParticleGun>(1); */
    /* auto* table = G4ParticleTable::GetParticleTable(); */
    /* gun->SetParticleDefinition( */
        /* table->FindParticle("gamma")); */

    /* gun->SetParticleEnergy(30 * keV); */
    /* gun->SetParticlePosition(G4ThreeVector(3, 3, 22));//ImpDetectorConstruction::maxYCoordCm())); */
    /* gun->SetParticleMomentumDirection(G4ThreeVector(0, 0, -1)); */
}

// shared_ptr auto deletes
ImpPrimaryGeneratorAction::~ImpPrimaryGeneratorAction()
{ }

void ImpPrimaryGeneratorAction::GeneratePrimaries(G4Event* evt)
{
    gps->GeneratePrimaryVertex(evt);
}
