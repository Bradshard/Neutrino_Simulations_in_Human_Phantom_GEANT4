#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"
#include "G4ParticleTable.hh"
#include "G4NeutrinoE.hh"
#include "G4NeutrinoMu.hh"
#include "G4NeutrinoTau.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4MuonMinus.hh"
#include "G4MuonPlus.hh"
#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4Event.hh"
#include "G4Run.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "Randomize.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

int main(int argc,char** argv)
{
  // Construct the run manager
  G4RunManager* runManager = new G4RunManager;

  // Set up the physics list
  G4PhysListFactory factory;
  G4VModularPhysicsList* physList = factory.GetReferencePhysList("FTFP_BERT");
  physList->RegisterPhysics(new G4DecayPhysics());
  physList->RegisterPhysics(new G4RadioactiveDecayPhysics());
  physList->RegisterPhysics(new G4EmStandardPhysics());
  runManager->SetUserInitialization(physList);

  // Set up the primary generator action
  G4ParticleGun* gun = new G4ParticleGun();
  gun->SetParticleDefinition(G4NeutrinoE::Definition());
  gun->SetParticleEnergy(10*MeV);
  gun->SetParticlePosition(G4ThreeVector(0,0,0));
  runManager->SetUserAction(gun);

  // Initialize the run manager
  runManager->Initialize();

  // Initialize the visualization
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();

  // Get the UI manager
  G4UImanager* UI = G4UImanager::GetUIpointer();

  // Set up the run control
  G4int numberOfEvents = 10;
  G4int randomSeed = 0;
  UI->ApplyCommand("/run/verbose 1");
  UI->ApplyCommand("/run/initialize");
  UI->ApplyCommand("/random/setSeed " + std::to_string(randomSeed));
  UI->ApplyCommand("/run/beamOn " + std::to_string(numberOfEvents));

  // Delete the run manager and visualization manager
  delete runManager;
  delete visManager;

  return 0;
}
