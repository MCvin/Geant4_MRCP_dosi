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
// TETPrimaryGeneratorAction.cc
// \file   MRCP_GEANT4/Internal/src/TETPrimaryGeneratorAction.cc
// \author Haegin Han
//

#include "TETPrimaryGeneratorAction.hh"

TETPrimaryGeneratorAction::TETPrimaryGeneratorAction(TETModelImport* tetData, G4int internalSource)
:G4VUserPrimaryGeneratorAction()
{
	// Find the tetrahedrons to be the source
	//
	G4double xMin(DBL_MAX), yMin(DBL_MAX), zMin(DBL_MAX);
	G4double xMax(DBL_MIN), yMax(DBL_MIN), zMax(DBL_MIN);
	for(G4int i=0;i<tetData->GetNumTetrahedron();i++){
		if(tetData->GetMaterialIndex(i) != internalSource) continue;
		G4Tet* tetSolid = tetData->GetTetrahedron(i);
		for(auto vertex:tetSolid->GetVertices()){
			if      (vertex.getX() < xMin) xMin = vertex.getX();
			else if (vertex.getX() > xMax) xMax = vertex.getX();
			if      (vertex.getY() < yMin) yMin = vertex.getY();
			else if (vertex.getY() > yMax) yMax = vertex.getY();
			if      (vertex.getZ() < zMin) zMin = vertex.getZ();
			else if (vertex.getZ() > zMax) zMax = vertex.getZ();
		}
		internalTetVec.push_back(tetData->GetTetrahedron(i));
	}

	if(internalTetVec.empty())
		G4Exception("TETPrimaryGeneratorAction::TETPrimaryGeneratorAction","",FatalErrorInArgument,
				     G4String("There is no tetrahedron for internal source").c_str());

	bBoxMin = G4ThreeVector(xMin, yMin, zMin);
	bBoxDim = G4ThreeVector(xMax-xMin, yMax-yMin, zMax-zMin);

	// initialise particle gun
	//
	fParticleGun = new G4ParticleGun(1);
	G4ParticleDefinition* particle
		= G4ParticleTable::GetParticleTable()->FindParticle("gamma");
	fParticleGun->SetParticleDefinition(particle);
	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(1., 0., 0.));
	fParticleGun->SetParticleEnergy(1.*MeV);
}

TETPrimaryGeneratorAction::~TETPrimaryGeneratorAction()
{
    delete fParticleGun;
}

void TETPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	// Sampling the position of the source (rejection method)
	//
	G4bool        insideChk(false);
	G4ThreeVector pos;
	do{
		pos = G4ThreeVector(bBoxMin.getX()+bBoxDim.getX()*G4UniformRand(),
		                    bBoxMin.getY()+bBoxDim.getY()*G4UniformRand(),
		                    bBoxMin.getZ()+bBoxDim.getZ()*G4UniformRand());

		for(auto tet:internalTetVec){
			if(tet->Inside(pos) == kOutside) continue;
			insideChk = true;
			break;
		}
	}while(!insideChk);

	// set the position and direction of a primary particle, and generate it
	fParticleGun->SetParticlePosition(pos);
	fParticleGun->SetParticleMomentumDirection(G4RandomDirection());
	fParticleGun->GeneratePrimaryVertex(anEvent);
}

