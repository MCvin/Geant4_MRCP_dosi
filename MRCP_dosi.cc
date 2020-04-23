//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// MRCP_dosi.cc
// \file   Geant4_MRCP_dosi/MRCP_dosi.cc
// \author Maxime Chauvin chauvin.maxime@gmail.com
// based on Internal.cc by Haegin Han

#include "TETDetectorConstruction.hh"
#include "TETModelImport.hh"
#include "TETPhysicsList.hh"
#include "TETActionInitialization.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "Randomize.hh"

int PrintUsage() {
	G4cerr << "Not the correct arguments. Usage:" << G4endl;
    G4cerr << " - interactive mode: ./MRCP_dosi -m [MODEL] -s [SOURCE ID]" << G4endl;
    G4cerr << " - batch mode: ./MRCP_dosi -m [MODEL] -s [SOURCE ID] -i [INPUT MACRO] -o [OUTPUT]" << G4endl;
    G4cerr << "Example: ./MRCP_dosi -m AF -s 9500" << G4endl;
    G4cerr << "Example: ./MRCP_dosi -m AF -s 9500 -i ../mac/run.mac -o ../output/out.dat" << G4endl;
	return 1;
}

int main(int argc, char** argv) {
	// Read the arguments for batch mode
	//
	G4bool   isAF(true);
	G4int    internalSource(-1);
	G4String macro;
	G4String output;
	G4int    n_arg(0);

	if ((argc != 5) and (argc != 9)) return PrintUsage();
	for ( G4int i = 1; i < argc; i++ ) {
		// model
		if ( G4String(argv[i]) == "-m" ) {
			if ( G4String(argv[i+1]) == "AF" ) {
				isAF = true;
			}
			else if ( G4String(argv[i+1]) == "AM" ) {
				isAF = false;
			}
			else {
				return PrintUsage();
			}
			n_arg += 1;
			i++;
		}
		// ID of the source organ
		else if ( G4String(argv[i]) == "-s" ) {
			internalSource = (G4int)(std::atoi(argv[i+1]));
			n_arg += 10;
			i++;
		}
		// input macro file name
		else if ( G4String(argv[i]) == "-i" ) {
			macro = argv[i+1];
			n_arg += 100;
			i++;
		}
		// output file name
		else if ( G4String(argv[i]) == "-o" ) {
			output = argv[i+1];
			n_arg += 1000;
			i++;
		}
		else {
			return PrintUsage();
		}
	}
	// check the arguments (11: interactive mode, 1111: batch mode)
	if (( n_arg != 11 ) and ( n_arg != 1111 )) {
		return PrintUsage();
	}

	// Construct the default run manager
	#ifdef G4MULTITHREADED
		G4MTRunManager * runManager = new G4MTRunManager;
		// set the default number of threads as one
		runManager->SetNumberOfThreads(1);
	#else
		G4RunManager * runManager = new G4RunManager;
	#endif

	// Set up the pseudo random generator (default: MixMax best for MT jobs)
	G4Random::setTheSeed(time(0));                                          // set seed(G4long)
	G4cout << "Random engine seed:  " << G4Random::getTheSeed() << G4endl;  // get current seed
	G4Random::showEngineStatus();

	// Set a class to import phantom data
	TETModelImport* tetData = new TETModelImport(isAF);

	// Set mandatory initialisation classes
	// detector construction
	runManager->SetUserInitialization(new TETDetectorConstruction(tetData));
	// physics list
	runManager->SetUserInitialization(new TETPhysicsList());
	// user action initialisation
	runManager->SetUserInitialization(new TETActionInitialization(tetData, internalSource, output));

	// Get the pointer to the User Interface manager
	G4UImanager* uiManager = G4UImanager::GetUIpointer();

	// Start UI session or process macro
	if ( n_arg == 11 ) {  // interactive mode if only -m and -s arguments
		G4VisManager* visManager = new G4VisExecutive;
		G4UIExecutive* uiExecutive = new G4UIExecutive(argc, argv, "csh");
//		G4UIExecutive* uiExecutive = new G4UIExecutive(argc, argv); // for Qt window
		visManager->Initialise();
		uiManager->ApplyCommand("/control/execute ../mac/vis.mac");
		uiExecutive->SessionStart();
		delete visManager;
		delete uiExecutive;
	}
	else if ( n_arg == 1111 ) {  // batch mode if exactly each of the 4 arguments -m -s -i -o
		G4String command = "/control/execute ";
		uiManager->ApplyCommand(command + macro);
	}
	else {
		PrintUsage();
	}

	// Job termination
	delete runManager;
	return 0;
}


