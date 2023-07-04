#include <stdio.h>
#include "G4VUserDetectorConstruction.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

class MyDetectorConstruction : public G4VUserDetectorConstruction {
public:
  G4VPhysicalVolume* Construct() {
    // Define material
    G4NistManager* nistManager = G4NistManager::Instance();
    G4Material* tissue = nistManager->FindOrBuildMaterial("G4_TISSUE_SOFT_ICRP");

    // Define dimensions
    G4double worldSizeXY = 10.0 * CLHEP::cm;
    G4double worldSizeZ = 10.0 * CLHEP::cm;
    G4double detectorThickness = 1.0 * CLHEP::cm;
      
    // Define the material properties for air
    G4double density = 1.29 * CLHEP::mg / CLHEP::cm3;  // Example density value for air
    G4double pressure = 1.0 * CLHEP::atmosphere;  // Example pressure value for air
    G4double temperature = 293.15 * CLHEP::kelvin;  // Example temperature value for air

    // Create the world volume
    G4Box* worldSolid = new G4Box("World", worldSizeXY, worldSizeXY, worldSizeZ);
    G4Material* airMaterial = new G4Material("G4_AIR", density, 2);
    airMaterial->AddElement(G4Element::GetElement("N"), 0.7);  // Example composition percentage for nitrogen
    airMaterial->AddElement(G4Element::GetElement("O"), 0.3);  // Example composition percentage for oxygen
    G4MaterialPropertiesTable* airMPT = new G4MaterialPropertiesTable();
    airMPT->AddConstProperty("PRESSURE",pressure, true);
    airMPT->AddConstProperty("TEMPERATURE",temperature, true);
    airMaterial->SetMaterialPropertiesTable(airMPT);
    G4LogicalVolume* worldLogical = new G4LogicalVolume(worldSolid, airMaterial, "World");
    G4VPhysicalVolume* worldPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), worldLogical, "World", nullptr, false, 0);

    // Create the detector volume
    G4Box* detectorSolid = new G4Box("Detector", worldSizeXY, worldSizeXY, detectorThickness);
    G4LogicalVolume* detectorLogical = new G4LogicalVolume(detectorSolid, tissue, "Detector");
    new G4PVPlacement(nullptr, G4ThreeVector(0.0, 0.0, -detectorThickness / 2.0), detectorLogical, "Detector", worldLogical, false, 0);

    return worldPhysical;
  }
};