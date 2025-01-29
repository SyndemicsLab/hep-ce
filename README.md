# HEP-CE: A Hepatitis C Virus Microsimulation
<a href="https://www.syndemicslab.org/hep-ce"><img align="right" src="https://github.com/SyndemicsLab/.github/blob/main/profile/images/HEPCE.png" alt="HEP-CE Logo" height="150" /></a>

The `HEP-CE` (**Hep**atitis C **C**ost **E**ffectiveness) model is a Markov chain Monte Carlo health state-transition model which simulates the spread and treatment of Hepatitis C Virus (HCV) in the United States.
The model uses values and information derived from a variety of sources, including clinical data and relevant literature.
HEP-CE is used to model the efficacy and cost-effectiveness of treatments, policies, and interventions aimed at controlling the HCV epidemic on a population-wide basis.

## Table of Contents
- [What's New](#whats-new)
- [Build Instructions](#build-instructions)

## What's New
The `HEP-CE` model you see here is a rewrite of [an earlier version](https://github.com/SyndemicsLab/hep-ce) of the model. This new, improved version attempts to improve upon key pain points in the previous incarnation, namely:

- Converting to a Discrete Event Simulation structure
- Improving readability
- Simplify input file structure

### Changes to Control Flow
This recapitulation of `HEP-CE` iterates across timesteps and events rather than over person lives, as past versions did. Each timestep (month), the simulated population is subjected to discrete "events". Measurables are stored at the individual Person level.

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

## Build Instructions

### Dependencies

- [DataManagement](https://github.com/SyndemicsLab/DataManagement)
- [GoogleTest/`gtest`](https://github.com/google/googletest) (optional, used
  for unit testing)

### Unix-based Systems

```sh
git clone git@github.com:SyndemicsLab/HEPCESimulationv2
scripts/build.sh
```
