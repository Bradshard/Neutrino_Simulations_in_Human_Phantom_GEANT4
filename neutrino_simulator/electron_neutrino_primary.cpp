//
//  g4_neutrinos.cpp
//  neutrino_sim
//
//  Created by burkan bereketoğlu on 30.06.2023.
//

#include <stdio.h>
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "G4VUserDetectorConstruction.hh"
#include "G4VUserPhysicsList.hh"
#include "G4UserEventAction.hh"
#include "G4VUserPrimaryGeneratorAction.hh"

#include "G4NeutrinoE.hh"
#include "G4ParticleGun.hh"
#include "G4Event.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"


#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "TFile.h"
#include "TTree.h"

int main(int argc, char** argv) {
  // Construct the run manager
  G4RunManager* runManager = new G4RunManager();

  // Set mandatory initialization classes
  MyDetectorConstruction* detector = new MyDetectorConstruction();
  runManager->SetUserInitialization(detector);

  // Initialize physics
  G4VUserPhysicsList* physicsList = new MyPhysicsList();
  runManager->SetUserInitialization(physicsList);

  // Set user action classes
  G4UserEventAction* eventAction = new MyEventAction();
  runManager->SetUserAction(eventAction);

  G4VUserPrimaryGeneratorAction* primaryGeneratorAction = new MyPrimaryGeneratorAction();
  runManager->SetUserAction(primaryGeneratorAction);

  // Initialize G4 kernel
  runManager->Initialize();

  // Visualization
  G4VisManager* visManager = new G4VisExecutive();
  visManager->Initialize();

  // UI session
  G4UIExecutive* ui = new G4UIExecutive(argc, argv);
  G4UImanager* uiManager = G4UImanager::GetUIpointer();
  uiManager->ApplyCommand("/control/execute init_vis.mac");

  // Run
  int numberOfEvents = 10;
  runManager->BeamOn(numberOfEvents);
    
  // Save data to ROOT file, get the event action and call WriteDataToRootFile
  const MyEventAction* myEventAction = dynamic_cast<const MyEventAction*>(runManager->GetUserEventAction());
  if (myEventAction) {
      myEventAction->WriteDataToRootFile("flux_data.root");
  } else {
      if (myEventAction) {
      // A different event action is set, but it's not of type MyEventAction
      // Handle the case when a different event action is set
      // G4String actionClassName = myEventAction->GetUserActionName();
      G4String expectedClassName = typeid(MyEventAction).name();
      G4cerr << "Error: Unexpected event action type. Expected: " << expectedClassName << ", Found: Undefined" << G4endl;
      } else {
      // No event action is set
      // Handle the case when no event action is set
      G4cerr << "Error: No event action is set." << G4endl;
      }
  }

  // Terminate
  delete ui;
  delete visManager;
  delete runManager;

  return 0;
}
