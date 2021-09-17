#pragma once

#include <cerrno>
#include <fstream>
#include <memory>
#include <sstream>

#include <G4VUserPrimaryGeneratorAction.hh>
#include <G4ParticleGun.hh>

#include <ImpEnergyPicker.hh>
#include <ImpSurfacePointPicker.hh>

class G4Box;
class G4Tubs;
class G4Event;

class ImpPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
    public:
        ImpPrimaryGeneratorAction();
        ImpPrimaryGeneratorAction(const G4Box* b, const G4String& flareSize);
        ImpPrimaryGeneratorAction(const G4Tubs* c, const G4String& flareSize);
        ~ImpPrimaryGeneratorAction();

        void GeneratePrimaries(G4Event* anEvent) override;
    private:
        std::unique_ptr<G4ParticleGun> gun;
        std::unique_ptr<ImpEnergyPicker> energyPicker;
        std::unique_ptr<ImpSurfacePointPicker> pointPicker;
        /* std::unique_ptr<G4GeneralParticleSource> gps; */
};
