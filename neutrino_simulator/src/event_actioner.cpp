#include <stdio.h>
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UserEventAction.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "TFile.h"
#include "TTree.h"

#include "G4Event.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"


class MyEventAction : public G4UserEventAction {
public:
  MyEventAction() {}

  void BeginOfEventAction(const G4Event*) {
    // Clear previous event data
    fluxX.clear();
    fluxY.clear();
    fluxZ.clear();
  }

  void EndOfEventAction(const G4Event* event) {
    // Get the primary vertex
    G4PrimaryVertex* primaryVertex = event->GetPrimaryVertex();
    if (!primaryVertex)
      return;

    // Iterate over primary particles
    G4PrimaryParticle* primaryParticle = primaryVertex->GetPrimary();
    while (primaryParticle) {
      // Accumulate momentum in x, y, and z directions
      G4ThreeVector momentum = primaryParticle->GetMomentum();
      fluxX.push_back(momentum.x());
      fluxY.push_back(momentum.y());
      fluxZ.push_back(momentum.z());

      primaryParticle = primaryParticle->GetNext();
    }
  }

  void WriteDataToRootFile(const std::string& filename) const {
    std::string outputPath = "./"; // specify the desired output
      
    // Construct the full path including the output directory
    std::string fullFilePath = outputPath + filename;
      
    TFile* file = new TFile(fullFilePath.c_str(), "RECREATE");
    TTree* tree = new TTree("flux_tree", "Flux Data");

    // Create branches in the ROOT tree
    std::vector<G4double> fluxXArray;
    std::vector<G4double> fluxYArray;
    std::vector<G4double> fluxZArray;
    tree->Branch("fluxX", &fluxXArray);
    tree->Branch("fluxY", &fluxYArray);
    tree->Branch("fluxZ", &fluxZArray);

    // Fill the branches with the flux data
    fluxXArray = fluxX;
    fluxYArray = fluxY;
    fluxZArray = fluxZ;

    // Write the ROOT tree to the file
    file->Write();
    file->Close();

    delete file;
  }

private:
  std::vector<G4double> fluxX;
  std::vector<G4double> fluxY;
  std::vector<G4double> fluxZ;
};