#include <sstream>

#include "G4Threading.hh"

#include "ImpAnalysis.hh"
#include "ImpRunAction.hh"

ImpRunAction::ImpRunAction() :
    G4UserRunAction()
{
}

ImpRunAction::~ImpRunAction()
{ }

void ImpRunAction::BeginOfRunAction(const G4Run*)
{
    ImpAnalysis::instance()->bookTuplesHistograms(IsMaster());
}

void ImpRunAction::EndOfRunAction(const G4Run*)
{
    ImpAnalysis::instance()->saveFiles(IsMaster());
}
