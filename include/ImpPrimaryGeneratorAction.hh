#pragma once

#include <cerrno>
#include <fstream>
#include <memory>
#include <sstream>

#include "G4VUserPrimaryGeneratorAction.hh"
/* #include "G4ParticleGun.hh" */
/* #include "G4ParticleTable.hh" */
/* #include "G4SystemOfUnits.hh" */


class G4Box;
class G4GeneralParticleSource;
class G4Event;

class ImpPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
    public:
        ImpPrimaryGeneratorAction();
        ~ImpPrimaryGeneratorAction();

        void GeneratePrimaries(G4Event* anEvent) override;
    private:
        std::unique_ptr<G4GeneralParticleSource> gps;
};
