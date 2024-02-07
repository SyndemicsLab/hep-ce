<p align="center">
<a href="https://www.syndemicslab.org/hep-ce"><img src="https://github.com/SyndemicsLab/.github/blob/main/profile/images/HEPCE.png" alt="HEP-CE Logo" height="100" /></a>
</p>

The `HEP-CE` (**Hep**atitis C **C**ost **E**ffectiveness) model is a Monte-Carlo health state-transition model which simulates the spread and treatment of Hepatitis C Virus (HCV) in the United States.
The model uses values and information derived from a variety of sources, including clinical data and relevant literature.
HEP-CE is used to model the efficacy and cost-effectiveness of treatments, policies, and interventions aimed at controlling the HCV epidemic on a population-wide basis.

## Table of Contents
- [What's New](#whats-new)
- [Build Instructions](#build-instructions)
- [Roadmap](#roadmap)

## What's New
The `HEP-CE` model you see here is a rewrite of [an earlier version](https://github.com/SyndemicsLab/hep-ce) of the model. This new, improved version attempts to improve upon key pain points in the previous incarnation, namely:

- Converting to a Discrete Event Simulation structure
- Improving readability
- Simplify input file structure

### Changes to Control Flow
This recapitulation of `HEP-CE` iterates across timesteps and events rather than over person lives, as past versions did. Each timestep (month), the simulated population is subjected to discrete "events".

At the top level, the model is broken down into three categories of events:

- Person-Level Events
- Clinical Events
- Calculation Events

These categories are further broken into discrete events:

- Person-Level Events:
  - Aging
  - Drug Behavior Changes
  - Acute HCV Clearance
  - Liver Disease Progression
  - Infections
  - Overdose
  - Death
- Clinical Events:
  - HCV Screening
  - Linking to Care
  - Voluntary Relinking to Care
  - Fibrosis Staging
  - Treatment
- Calculation Events:
  - Transmission (PreVenT)

### Input Structure
This version of the model aims to simplify input file structure.
Currently, the intended input folder structure is
```ini
# simple ini config
sim.conf
# tabular inputs
init_cohort.csv
antibody_testing.csv
background_costs.csv
background_mortality.csv
background_utilities.csv
behavior_costs.csv
behavior_transitions.csv
behavior_utilities.csv
fibrosis.csv
liver_disease_costs.csv
liver_disease_utilities.csv
incidence.csv
screening_and_linking.csv
smr.csv
```

## Build Instructions

### Dependencies

- [conan2](https://conan.io)
- [DataManagement](https://github.com/SyndemicsLab/DataManagement)
- [GoogleTest/`gtest`](https://github.com/google/googletest) (optional)

### Unix-based Systems

```sh
git clone git@github.com:SyndemicsLab/HEPCESimulationv2
scripts/debug-conanbuild.sh
```

### Windows

```bat
git clone git@github.com:SyndemicsLab/HEPCESimulationv2
cd scripts
debug-conanbuild.bat
```

## Roadmap

- [x] Rewrite Approval [2023-08-14]
- [x] Person-Level Events
- [ ] Clinical Events
- [ ] Calculation Events
- [x] Functional `main()` [2024-01-03]
