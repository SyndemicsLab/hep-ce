# HEP-CE Simulation

**Hep**atitis C **C**ost **E**ffectiveness (`HEP-CE`) is a health economics Markov Chain Monte Carlo micro-simulation model focused on studying the syndemic of opioid use disorder (OUD) and the hepatitis C virus (HCV).

## Building the Simulation

### Dependencies

- [DataManagement](https://github.com/SyndemicsLab/DataManagement)
- [conan2](https://conan.io)
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

## Model Flow

This recapitulation of `HEP-CE` iterates across timesteps and events rather than over person lives, as past versions did. Each timestep (month), the simulated population is subjected to discrete "events".

At the top level, the model is broken down into three categories of events:

1. Person-Level Events
2. Clinical Events
3. Calculation Events

Each person simulated in the model cohort iterates through these event categories in this order.
Each of these categories themself contains discrete events, explained in detail below.

### Person-Level Events

Person-level events are events external to the healthcare setting that can impact the likelihood or severity of an HCV infection, such as injection drug use, age, or, if already infected, HCV progression.

- [X] Aging
- [X] Overdose
- [X] Death & Fatal Overdose
- [X] Opioid Drug Use Behavior & MOUD
- [X] HCV Infection
- [X] HCV/Liver Progression (if already infected)
- [X] HCV Clearance

### Clinical Events

Clinical events are those within the healthcare setting, namely screening (testing), linkage, and treatment.

- [X] Intervention Screening
- [X] Background Screening
- [ ] HCC Screening
- [X] Linkage
- [X] Relinkage
- [ ] Treatment

### Calculation Events

- [ ] Cost
- [ ] "Time Spent In" Updates
- [ ] Transmission (Infection Rate Change)
