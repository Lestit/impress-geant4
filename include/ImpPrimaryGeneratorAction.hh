#pragma once

#include <memory>

#include "G4VUserPrimaryGeneratorAction.hh"
/* #include "G4ParticleGun.hh" */
/* #include "G4ParticleTable.hh" */
/* #include "G4SystemOfUnits.hh" */

#include "ImpDetectorConstruction.hh"

class G4GeneralParticleSource;
class G4Event;

class ImpPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
    public:
        ImpPrimaryGeneratorAction();
        ~ImpPrimaryGeneratorAction();

        void GeneratePrimaries(G4Event* anEvent) override;

    private:
        /* std::shared_ptr<G4ParticleGun> gun; */
        std::unique_ptr<G4GeneralParticleSource> gps;
        /* G4GeneralParticleSource* background; */
};
