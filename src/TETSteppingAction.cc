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
// TETSteppingAction.cc
// file   : Geant4_MRCP_dosi/src/TETSteppingAction.cc
// authors: Maxime Chauvin, Haegin Han
//

#include "TETSteppingAction.hh"

TETSteppingAction::TETSteppingAction()
    : G4UserSteppingAction(), kCarTolerance(1.0000000000000002e-07), stepCounter(0), checkFlag(0)
{
}

TETSteppingAction::~TETSteppingAction()
{
}

void TETSteppingAction::UserSteppingAction(const G4Step *step)
{
    // Slightly move the particle when the step length of five continuous steps is
    // shorter than the tolerance (0.1 nm)
    //
    G4Track *theTrack = step->GetTrack();
    G4bool CheckingLength = (step->GetStepLength() < kCarTolerance);
    if (CheckingLength)
    {
        ++stepCounter;
        if (checkFlag && stepCounter >= 5)
        {
            // kill the track if the particle is stuck even after the slight move
            // (this hardly occurs) -> ! dangerous: do we want this ?
            theTrack->SetTrackStatus(fStopAndKill);
            stepCounter = 0;
            checkFlag = 0;
        }
        else if (stepCounter >= 5)
        {
            // if a particle is at the same position (step length < 0.1 nm) for five consecutive steps,
            // slightly move (0.1 nm) the stuck particle in the direction of momentum
            theTrack->SetPosition(theTrack->GetPosition() + theTrack->GetMomentumDirection() * kCarTolerance);
            checkFlag = 1;
        }
    }
    else
        stepCounter = 0;
}
