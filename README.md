# Geant4_MRCP_dosi

                                MRCP_dosi - internal dosimetry calculations
                            with the ICRP Mesh Reference Computational Phantoms
                                                ----------

Authors: Maxime Chauvin
based on code developed by Haegin Han

## 1 - INTRODUCTION

Geant4_MRCP_dosi is a Geant4 application for internal dosimetry calculations 
with the ICRP mesh-type adult (male and female) reference computationnal models.

The models are not tracked by Git so you have to place the following files in 
the data/models/ folder:
- MRCP_AF.ele   (384M)
- MRCP_AF.node  (80M)
- MRCP_AM.ele   (367M)
- MRCP_AM.node  (64M)

## 2 - GEOMETRY DEFINITION

The geometry is build from the tetrahedral mesh models provided in the folder 
data/models/

The models are loaded into Geant4 as tetrahedrons using the G4Tet class:
```c++
// save the element (tetrahedron) data as the form of std::vector<G4Tet*>
tetVector.push_back(new G4Tet("Tet_Solid",
                              vertexVector[ele[0]],
                              vertexVector[ele[1]],
                              vertexVector[ele[2]],
                              vertexVector[ele[3]]));
```

## 3 - PHYSICS LIST

The physics list contains the following PhysicsList classes as Geant4 
modular physics list with registered physics builders provided in Geant4:
```c++
// EM physics
RegisterPhysics(new G4EmLivermorePhysics());

// Decay
RegisterPhysics(new G4DecayPhysics());

// Radioactive decay
RegisterPhysics(new G4RadioactiveDecayPhysics());
```
 	 
## 4 - SOURCE

- source organ can be defined by using -i option.
- source particle and the particle energy can be specified
in source.mac file by using macro commands for 
G4ParticlGun (/gun/).

   The generation of particule is done using the new General Particle Source. 
   Documentation for this can be found in:
   http://reat.space.qinetiq.com/gps/
   The default beam set in vis1.mac and run1.mac is a parallele beam of 
   gamma photons directed towards the instrument, along Z.
   The energy is set by the command /gps/ene/mono 59.5 keV
   and the polarization angle by /gps/polarization 1. 0. 0. # 0 deg
   
## 5 - SCORER AND OUTPUT
example.out file in each folder is the result file for example.in. The result
  file provides absorbed doses for all organ IDs for TM model listed in Appendix A.

   A HIT is a record, track per track (even step per step), of all the 
   informations needed to simulate and analyse the detector response.
   
   In PoGOSim the slow, fast and BGO scintillators are considered as the 
   detector. Therefore they are declared 'sensitive detectors' (SD) in
   the DetectorConstruction class. 
   
   Then, a Hit is defined as a set of 4 informations per step, inside 
   the chambers, namely:
    (- the track identifier (an integer),
    - the chamber number,
    - the total energy deposit in this step,
    - the position of the deposit.)
   
   A given hit is an instance of the class TrackerHit which is created 
   during the tracking of a particle, step by step, in the method 
   TrackerSD::ProcessHits(). This hit is inserted in a HitsCollection.

   EventAction::EndOfEventAction() collects informations event per event
   from the hits collections, and print every hits in a file (via the method 
   EventAction::PrintHitsCollection(TrackerHitsCollection* HC, G4String name)).

## 6 - PSEUDO RANDOM GENERATOR

It is important to note that the (default) MixMax random number generator 
(available since version 10.3) is the recommended engine for MT jobs since it 
guarantees divergent number histories even for consecutive random number seeds.

See [Random Number Generation Seeding in MT](http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForToolkitDeveloper/html/OOAnalysisDesign/Multithreading/mt.html#random-number-generation-seeding-in-mt)

## A - VISUALISATION

The visualization manager is set via the G4VisExecutive class
in the main() function in the MRCP_dosi.cc file.

The initialisation of the drawing is done via a set of /vis/ commands
in the macro vis.mac. 

This macro is automatically read from the main function when the application is 
run in interactive mode.

The tracks are automatically drawn at the end of each event, accumulated
for all events and erased at the beginning of the next run.

Note:

The memory required for the visualisation is ~35 GB when the code is
run on a single thread. The application requires less than 10 GB in batch mode 
without visualisation.

## B - HOW TO BUILD

You need to have the G4 environment variables set up in your terminal session. 
If it is not done in your .bashrc or .profile file you need to do this first 
with your own Geant4 install path:
    ```bash
    source /usr/share/geant4/geant4-install/bin/geant4.sh
    ```
Go to the build directory and run:
    ```bash
    cmake ../
    make
    ```

## C - HOW TO RUN

Execute MRCP_dosi in interactive mode with visualization:
    ```
    ./MRCP_dosi -m [MODEL] -s [SOURCE ID]
    ./MRCP_dosi -m AF -s 9500
    ```
    and type in the commands you want line by line:
    ```
    Idle> /tracking/verbose 2
    Idle> ...
    Idle> exit
    ```

Execute MRCP_dosi in batch mode with macro files 
    (without visualization):
    ```
    ./MRCP_dosi -m [MODEL] -s [SOURCE ID] -i [INPUT MACRO] -o [OUTPUT]
    ./MRCP_dosi -m AF -s 9500 -i ../mac/run.mac -o ../output/output.dat
    ```

## D - MULTI-THREADED MODE

This application can be run in multi-threaded mode if Geant4 was 
compiled in multi-threaded mode.

The number of threads can be set in the input macro file (see mac/run.mac)
