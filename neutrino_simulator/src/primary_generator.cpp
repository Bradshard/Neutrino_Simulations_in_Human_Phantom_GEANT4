#include "G4ParticleGun.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"

class MyPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
  MyPrimaryGeneratorAction() {
    // Create a neutrino source particle gun
    particleGun = new G4ParticleGun();
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* neutrino = particleTable->FindParticle("nu_e");
    particleGun->SetParticleDefinition(neutrino);
    particleGun->SetParticleEnergy(1.0 * GeV);
  }

  ~MyPrimaryGeneratorAction() {
    delete particleGun;
  }

  void GeneratePrimaries(G4Event* event) {
    // Set the neutrino source position and direction
    particleGun->SetParticlePosition(G4ThreeVector(0.0, 0.0, 10.0 * CLHEP::cm));
    particleGun->SetParticleMomentumDirection(G4ThreeVector(0.0, 0.0, -1.0));

    // Generate the primary particle
    particleGun->GeneratePrimaryVertex(event);
  }

private:
  G4ParticleGun* particleGun;
};
