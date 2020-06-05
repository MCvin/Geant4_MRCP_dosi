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
// TETRunAction.cc
// file   : Geant4_MRCP_dosi/src/TETRunAction.cc
// authors: Maxime Chauvin, Haegin Han
//

#include "TETRunAction.hh"

TETRunAction::TETRunAction(TETModelImport *_tetData, G4String _output)
    : tetData(_tetData), fRun(0), nEvents(0), runID(0), outputFile(_output)
{
}

TETRunAction::~TETRunAction()
{
}

G4Run *TETRunAction::GenerateRun()
{
    // generate run
    fRun = new TETRun();
    return fRun;
}

void TETRunAction::BeginOfRunAction(const G4Run *aRun)
{
    // print the progress at the interval of 10%
    nEvents = aRun->GetNumberOfEventToBeProcessed();
    G4RunManager::GetRunManager()->SetPrintProgress(int(nEvents * 0.1));
}

void TETRunAction::EndOfRunAction(const G4Run *aRun)
{
    // print the result only in the Master
    if (!isMaster)
        return;

    // get the run ID
    runID = aRun->GetRunID();

    // print the run result by G4cout and std::ofstream
    PrintResult(G4cout);

    std::ofstream ofs(outputFile.c_str());
    PrintResult(ofs);
    ofs.close();
}

void TETRunAction::PrintResult(std::ostream &out)
{
    // Print run result
    using namespace std;
    EDEPMAP edepMap = *fRun->GetEdepMap();

    out << G4endl
        << "===============================================================================================================================" << G4endl
        << " Run #" << runID << " / Number of event processed : " << nEvents << G4endl
        << "===============================================================================================================================" << G4endl
        << "organ ID|"
        << setw(18) << "Mass (g)"
        << setw(18) << "Edep (MeV)"
        << setw(14) << "Edep_SD"
        << setw(18) << "Edep_squared"
        << setw(18) << "Dose (Gy)"
        << setw(14) << "Dose_SD"
        << setw(18) << "Dose_squared" << G4endl;
    // ToDo: add n_hits and n_events_hits like in DoseByRegions from GATE for fluence estimation
    out.precision(10);
    for (auto itr : tetData->GetMassMap())
    {
        G4double edep = edepMap[itr.first].first;
        G4double edep_sq = edepMap[itr.first].second;
        G4double edep_var = ((edep_sq / nEvents) - (edep * edep / (nEvents * nEvents))) / (nEvents - 1);
        G4double edep_sd = sqrt(edep_var) / (edep / nEvents);

        G4double dose = edepMap[itr.first].first / itr.second;
        G4double dose_sq = edepMap[itr.first].second / (itr.second * itr.second);
        G4double dose_var = ((dose_sq / nEvents) - (dose * dose / (nEvents * nEvents))) / (nEvents - 1);
        G4double dose_sd = sqrt(dose_var) / (dose / nEvents);

        out << setw(8)  << itr.first << "|"
            << setw(18) << scientific << itr.second / g
            << setw(18) << scientific << edep / MeV
            << setw(14) << fixed << edep_sd
            << setw(18) << scientific << edep_sq / MeV
            << setw(18) << scientific << dose / (joule / kg)
            << setw(14) << fixed << dose_sd
            << setw(18) << scientific << dose_sq / (joule / kg) << G4endl;
    }
    out << "===============================================================================================================================" << G4endl << G4endl;
}
