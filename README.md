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

The geometry is build from a tetrahedral mesh model specified by the -m (model) 
option followed by the model name (ex: -m AF).

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

The particles are generated from a region of the model specified by 
the -s (source) option followed by the ID of the source organ/tissue region.

The particle type and energy can be defined in the mac/run.mac input file 
with macro commands for G4ParticleGun:
```
/gun/particle  gamma
/gun/energy    1. MeV
```

ToDo: set the particle type and energy with command line arguments -p and -e

## 5 - SCORER AND OUTPUT

Energy deposits are scored inside the model and saved in a map with the 
associated organ/tissue ID where they occured.

The simulation ouput an ascii file which provides absorbed dose, uncertainty 
and masses for all organs/tissues of the model. The output filename is 
specified using the -o argument.

ToDo: add a new ouput with edep and absorbed dose for each tetrahedron.

## 6 - PSEUDO RANDOM GENERATOR

From [Random Number Generation Seeding in MT](http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForToolkitDeveloper/html/OOAnalysisDesign/Multithreading/mt.html#random-number-generation-seeding-in-mt):

>"It is important to note that the (default) MixMax random number generator 
(available since version 10.3) is the recommended engine for MT jobs since it 
guarantees divergent number histories even for consecutive random number seeds."

## A - VISUALISATION

The visualization manager is set via the G4VisExecutive class in the main() 
function of MRCP_dosi.cc.

The initialisation of the drawing is done via a set of /vis/ commands in the 
macro vis.mac. This macro is automatically read from the main function when 
the application is run in interactive mode.

The tracks are drawn at the end of each event, accumulated for 
all events and erased at the beginning of the next run.

Note: The memory required for the visualisation is ~35 GB when the code is
run on a single thread. The application requires less than 10 GB without 
visualisation (batch mode).

## B - HOW TO BUILD

You need to have the G4 environment variables set up in your terminal session. 
If it is not done in your .bashrc or .profile file you need to do this first 
with your own Geant4 install path:
```bash
source /usr/share/geant4/geant4-install/bin/geant4.sh
```
Create a build directory, and inside it run:
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

Execute MRCP_dosi in batch mode with macro files (without visualization):
```
./MRCP_dosi -m [MODEL] -s [SOURCE ID] -i [INPUT MACRO] -o [OUTPUT]
./MRCP_dosi -m AF -s 9500 -i ../mac/run.mac -o ../output/output.dat
```

## D - MULTITHREADED MODE

This application can (and should) be run in multithreaded mode if Geant4 was 
build with support for multithreading (GEANT4_BUILD_MULTITHREADED=ON). 
The number of threads can be set in the input macro file (see mac/run.mac).
