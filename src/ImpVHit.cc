#include <ImpVHit.hh>

#include <G4Circle.hh>
#include <G4Point3D.hh>
#include <G4SystemOfUnits.hh>
#include <G4VisAttributes.hh>
#include <G4VVisManager.hh>

ImpVHit::ImpVHit(const G4String& channelId, const G4ThreeVector& position, G4double arrivalTime) :
    assocChannelId(channelId),
    arrivalTime(arrivalTime),
    position(position)
{ }

ImpVHit::~ImpVHit() { }

void ImpVHit::Draw()
{
    auto* visMan = G4VVisManager::GetConcreteInstance();
    if (visMan == nullptr) return;

    G4Circle c(peekPosition());
    c.SetDiameter(G4VMarker::screen, 5);

    c.SetFillStyle(G4Circle::filled);
    G4VisAttributes a(G4Color(1, 0, 0));
    c.SetVisAttributes(a);
    visMan->Draw(c);
}
