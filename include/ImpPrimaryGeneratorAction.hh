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
        using DistributionType = ImpEnergyPicker::DistributionType;
        ImpPrimaryGeneratorAction();
        /* ImpPrimaryGeneratorAction(const G4Box* b, const G4String& flareSize); */
        /* ImpPrimaryGeneratorAction(const G4Tubs* c, const G4String& flareSize); */
        ~ImpPrimaryGeneratorAction();
        
        void updateWorldVolume(const G4Box* b);
        void updateWorldVolume(const G4Tubs* c);

        void GeneratePrimaries(G4Event* anEvent) override;
    private:
        std::unique_ptr<G4ParticleGun> gun;
        // sorry boss
        ImpEnergyPicker* energyPicker;
        std::unique_ptr<ImpSurfacePointPicker> pointPicker;
};
