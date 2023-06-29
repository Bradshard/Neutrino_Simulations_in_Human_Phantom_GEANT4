// to set up simulation geometry
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"

// to define physics processes
#include "G4ProcessManager.hh"
#include "G4NeutrinoNucleusProcess.hh"
#include "G4ProcessVector.hh"

// to generate primary particles
#include "G4ParticleGun.hh"
#include "G4NeutrinoE.hh"
#include "G4NeutrinoMu.hh"
#include "G4NeutrinoTau.hh"

// to simulate particle interactions
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4EventManager.hh"
#include "G4SDManager.hh"

// for to analysis of results 
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4THitsCollection.hh"
#include "G4HCtable.hh"
#include "G4RunManager.hh"
#include "G4ios.hh"
#include <fstream>


/*  here we define simulation geometry.*/
// Define the dimensions of your tissue sample
G4double tissue_size_x = 10.0 * cm;
G4double tissue_size_y = 10.0 * cm;
G4double tissue_size_z = 10.0 * cm;

// Define the material for your tissue sample
G4NistManager* nist = G4NistManager::Instance();
G4Material* tissue_material = nist->FindOrBuildMaterial("G4_TISSUE_SOFT_ICRP");

// Create a box-shaped volume for the tissue sample
G4Box* tissue_box = new G4Box("tissue_box", tissue_size_x / 2.0, tissue_size_y / 2.0, tissue_size_z / 2.0);

// Create a logical volume for the tissue sample using the tissue material and the box shape
G4LogicalVolume* tissue_logic = new G4LogicalVolume(tissue_box, tissue_material, "tissue_logic");

// Place the tissue sample in the world volume at the origin
new G4PVPlacement(0, G4ThreeVector(0, 0, 0), tissue_logic, "tissue_phys", world_logic, false, 0);

/*  here we show the physics processes.*/
// Get the process manager for the neutrino particle
G4ProcessManager* process_manager = G4Neutrino::Neutrino()->GetProcessManager();

// Define the neutrino-nucleus interaction process
G4NeutrinoNucleusProcess* nucl_int = new G4NeutrinoNucleusProcess();
process_manager->AddDiscreteProcess(nucl_int);

// Define the nuclear de-excitation process
G4ProcessVector* decay_processes = G4DecayPhysics::GetDecayProcessVector();
process_manager->SetProcessList(*decay_processes);


/*  here we define the primary particles.*/
// Define the particle gun
G4ParticleGun* particle_gun = new G4ParticleGun(1);

// Generate a neutrino event with random energy and direction
G4double energy = G4UniformRand() * 10.0 * MeV;
G4double cos_theta = G4UniformRand() * 2.0 - 1.0;
G4double phi = G4UniformRand() * 2.0 * pi;
G4ThreeVector direction(sin(acos(cos_theta)) * cos(phi), sin(acos(cos_theta)) * sin(phi), cos_theta);

// Choose the neutrino flavor based on the simulation parameters
G4ParticleDefinition* particle_def = nullptr;
if (neutrino_flavor == "nu_e") {
    particle_def = G4NeutrinoE::NeutrinoE();
} else if (neutrino_flavor == "nu_mu") {
    particle_def = G4NeutrinoMu::NeutrinoMu();
} else if (neutrino_flavor == "nu_tau") {
    particle_def = G4NeutrinoTau::NeutrinoTau();
} else {
    G4cerr << "Invalid neutrino flavor specified: " << neutrino_flavor << G4endl;
    return;
}

// Set the particle gun properties
particle_gun->SetParticleDefinition(neutrino_def);
particle_gun->SetParticleEnergy(energy);
particle_gun->SetParticleMomentumDirection(direction);

// Generate the primary particle
particle_gun->GeneratePrimaryVertex(anEvent);


/* We simulate particle interactions*/

// Create a run manager
G4RunManager* run_manager = new G4RunManager();

// Set the user-defined classes for the run manager
run_manager->SetUserInitialization(world_geo);
run_manager->SetUserInitialization(physics_list);
run_manager->SetUserAction(primary_gen);

// Initialize the run manager and the physics list
run_manager->Initialize();

// Get the event manager
G4EventManager* event_manager = G4EventManager::GetEventManager();

// Get the sensitive detector manager
G4SDManager* sd_manager = G4SDManager::GetSDMpointer();

// Open the output file
std::ofstream output_file("output.txt");

// Define a 3D histogram to store the flux at each vertex
const int num_bins = 100; // Adjust the number of bins as per your requirement
G4double bin_size_x = tissue_size_x / num_bins;
G4double bin_size_y = tissue_size_y / num_bins;
G4double bin_size_z = tissue_size_z / num_bins;
G4double flux_histogram[num_bins][num_bins][num_bins] = {0.0};

// Loop over the desired number of events
for (G4int i = 0; i < num_events; i++) {

    // Generate the primary particle
    particle_gun->GeneratePrimaryVertex(anEvent);

    // Simulate the particle interactions and transport
    run_manager->BeamOn(1);

    // Retrieve the position of the primary particle (vertex)
    G4ThreeVector vertex_position = particle_gun->GetParticlePosition();

    // Calculate the bin indices for the vertex position
    G4int bin_index_x = static_cast<G4int>(vertex_position.x() / bin_size_x);
    G4int bin_index_y = static_cast<G4int>(vertex_position.y() / bin_size_y);
    G4int bin_index_z = static_cast<G4int>(vertex_position.z() / bin_size_z);

    // Accumulate the flux at the vertex position
    flux_histogram[bin_index_x][bin_index_y][bin_index_z]++;

    // Reset the event manager for the next event
    event_manager->CleanUp();
}

// Loop over the vertices in the flux histogram and write the flux values to the output file
for (G4int i = 0; i < num_bins; i++) {
    for (G4int j = 0; j < num_bins; j++) {
        for (G4int k = 0; k < num_bins; k++) {
            G4double flux = flux_histogram[i][j][k];
            G4double vertex_x = (i + 0.5) * bin_size_x;
            G4double vertex_y = (j + 0.5) * bin_size_y;
            G4double vertex_z = (k + 0.5) * bin_size_z;

            output_file << flux << " " << vertex_x << " " << vertex_y << " " << vertex_z << std::endl;
        }
    }
}

output_file.close();
