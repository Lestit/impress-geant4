#pragma once

#include "G4VUserActionInitialization.hh"

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
        ImpActionInitialization();
        ~ImpActionInitialization();

        void BuildForMaster() const override;
        void Build() const override;
};
