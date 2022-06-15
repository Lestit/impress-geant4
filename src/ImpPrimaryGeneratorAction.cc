#include <G4Event.hh>
#include <G4ParticleTable.hh>
#include <G4SystemOfUnits.hh>
#include <G4UImanager.hh>

#include <ImpAnalysis.hh>
#include <ImpPrimaryGeneratorAction.hh>

ImpPrimaryGeneratorAction::ImpPrimaryGeneratorAction()
        : G4VUserPrimaryGeneratorAction(),
    gps(std::make_unique<G4GeneralParticleSource>())
{ }

// unique_ptrs auto delete
ImpPrimaryGeneratorAction::~ImpPrimaryGeneratorAction()
{
}

void ImpPrimaryGeneratorAction::GeneratePrimaries(G4Event* evt)
{
    gps->GeneratePrimaryVertex(evt);
    double e = gps->GetParticleEnergy();
    ImpAnalysis::instance()->addIncidentEnergy(e);
}
