#include "G4VUserPhysicsList.hh"
#include "G4NeutrinoE.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

class MyPhysicsList : public G4VUserPhysicsList {
public:
  void ConstructParticle() {
    // Define the electron neutrino
    G4NeutrinoE::NeutrinoEDefinition();
  }

  void ConstructProcess() {
    // Do nothing, use default Geant4 physics processes
    //G4VUserPhysicsList::ConstructProcess();
      
   // If i will do I will put custom physics processes here.
  }
};
