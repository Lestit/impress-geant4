#pragma once

#include "G4UserRunAction.hh"

class G4Run;

class ImpRunAction : public G4UserRunAction
{
    public:
        ImpRunAction();
        ~ImpRunAction();

        /* G4Run* GenerateRun() override; */
        void BeginOfRunAction(const G4Run*) override;
        void EndOfRunAction(const G4Run*) override;
};
