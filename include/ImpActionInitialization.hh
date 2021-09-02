#pragma once

#include "G4VUserActionInitialization.hh"

#include "ImpEventAction.hh"
#include "ImpDetectorConstruction.hh"
#include "ImpPrimaryGeneratorAction.hh"
#include "ImpRunAction.hh"

/*
 * eventually want:
 *     event action
 *     run action
 *     stacking action
 *     stepping action
 *     tracking action
 *     maybe more . . . ?
 */

class ImpActionInitialization : public G4VUserActionInitialization
{
    public:
        ImpActionInitialization(const ImpDetectorConstruction*);
        ~ImpActionInitialization();

        void BuildForMaster() const override;
        void Build() const override;

    private:
        const ImpDetectorConstruction* detCon;
};
