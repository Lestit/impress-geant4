#include <G4Box.hh>
#include <G4GeneralParticleSource.hh>
#include <G4Tubs.hh>
#include <G4SystemOfUnits.hh>
#include <G4UImanager.hh>

#include <ImpPrimaryGeneratorAction.hh>

ImpPrimaryGeneratorAction::ImpPrimaryGeneratorAction()
        : G4VUserPrimaryGeneratorAction(),
    gps(std::make_unique<G4GeneralParticleSource>())
{ }

// unique_ptr auto deletes
ImpPrimaryGeneratorAction::~ImpPrimaryGeneratorAction()
{ }

void ImpPrimaryGeneratorAction::GeneratePrimaries(G4Event* evt)
{
    gps->GeneratePrimaryVertex(evt);
}
