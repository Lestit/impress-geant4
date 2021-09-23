#pragma once

#include "G4Types.hh"
#include "G4String.hh"
#include "G4VUserActionInitialization.hh"
class ImpVDetectorConstruction;

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
        ImpActionInitialization(const ImpVDetectorConstruction* dc);
        ImpActionInitialization();
        ~ImpActionInitialization();

        void BuildForMaster() const override;
        void Build() const override;

    private:
        const ImpVDetectorConstruction* dc;
        void setupPrimaryGenerator() const;
};
