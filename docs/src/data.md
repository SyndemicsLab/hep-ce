# Managing the Data

The HEPCE model is packaged as a library, but for ease of use we do also provide a basic command line interface executable for users to run the model. To this end, we make use of command line arguments and required input structures.

## Required Inputs

There are two required input files that contain all the data we need. The first file is a configuration file titled `sim.conf`. The second is an SQLite Database file which contains multiple tables.

### sim.conf

The `sim.conf` file forms the backbone of the HEP-CE model. It governs the general event flow and the basic single-value parameters in the model. This file is meant to be changed by the users to determine what events occur to a person in the model and it does not contain tabular data. The general structure follows a prototypical `.ini` file with a header describing a set of key-value pairs. For HEPCE, there are 14 unique sets:

- simulation
- mortality
- infection
- eligibility
- fibrosis
- fibrosis_staging
- screening
- screening_background_ab
- screening_background_rna
- screening_intervention_ab
- screening_intervention_rna
- linking
- treatment
- cost

#### simulation

The simulation section governs the basic details of the simulation. An example is given below:

```ini
[simulation]
seed            = 1234  # Seed value to start the random number generator
population_size = 10000 # Number of people to run through the simulation
events          = Aging, Clearance, HCVInfections, Death 
duration        = 1200  # Time in months
start_time      = 0     # Used for checkpointing. Default: 0
```

The events value specified here is used to control which events execute in the simulation. The order is critical here as all the events occur sequentially within the same timestep; this means if you put "Clearance" after "Infections" you have the potential that people are infected and acutely clear in the same timestep. The default order of all events is:

```ini
event = Aging, Pregnancy BehaviorChanges, FibrosisProgression, HCVInfection, HCVScreening, HCVLinking, VoluntaryRelinking, FibrosisStaging, HCVTreatment, Death
```

#### mortality

The mortality section governs the details of non-stratified death rates due to various conditions. An example is given below:

```ini
[mortality]
f4_infected         = 0.0106842628666393    # Monthly probability of dying due to F4 fibrosis while infected with HCV 
f4_uninfected       = 0.000641055772        # Monthly probability of dying due to F4 fibrosis while not infected with HCV
decomp_infected     = 0.028923195           # Monthly probability of dying due to Decompensated fibrosis while infected with HCV
decomp_uninfected   = 0.0017353917          # Monthly probability of dying due to Decompensated fibrosis while not infected with HCV
```

#### infection

The infection section governs the details of non-stratified hepatitis-C infection parameters. An example is given below:

```ini
[infection]
clearance_prob      = 0.0489    # Monthly probability of acute clearance. If not provided, default to a rate of 25% over 6 months
genotype_three_prob = 0.153     # Probability of being genotype 3
```

#### eligibility

The eligibility governs the restrictions on recieving treatment for hepatitis-C. An example is given below:

```ini
[eligibility]
ineligible_drug_use                 =   # never, active_injection, active_noninjection, former_injection, former_noninjection
ineligible_fibrosis_stages          =   # f0, f1, f2, f3, f4, decomp, ehcc, lhcc
ineligible_time_former_threshold    =   # monthly timesteps
ineligible_pregnancy_states         =   # pregnant, restricted-postpartum, year-one-postpartum, year-two-postpartum
ineligible_time_since_linked        = 0 # monthly timesteps
```

#### fibrosis

The fibrosis section governs the probability of progressing from one state to the next. An example is given below:

```ini
[fibrosis]
f01 = 0.008877              # Probability of progressing from F0 to F1
f12 = 0.00681               # Probability of progressing from F1 to F2
f23 = 0.0097026             # Probability of progressing from F2 to F3
f34 = 0.0096201             # Probability of progressing from F3 to F4
f4d = 0.00558434922840212   # Probability of progressing from F4 to Decompensated
```

#### fibrosis_staging

The fibrosis_staging section governs the details around fibrosis staging. An example is given below:

```ini
[fibrosis_staging]
period                      = 1200      # how frequently we do staging, in months
test_one                    = idealtest # test one name, must match column in fibrosis table
test_one_cost               = 0         # cost of applying the first test

test_two                    =           # test two name, must match column in fibrosis table
test_two_cost               =           # cost of applying the second test

# if test_two is provided, all inputs in this section below this point must have values
multitest_result_method     =           # latest or maximum
test_two_eligible_stages    =           # which fibrosis stages can receive a second test
```

#### screening

The screening section governs the details around hepatitis-C screening. An example is given below:

```ini
[screening]
intervention_type   = periodic  # one-time, null, or periodic
period              = 12        # time between screens (only used if intervention periodic)
```

#### screening_background_ab

The screening_background_ab section governs the details around background antibody hepatitis-C screening. An example is given below:

```ini
[screening_background_ab]
cost                = 36.56 # The cost for applying a background antibody screen
acute_sensitivity   = 0.98  # The sensitivity for background antibody screening with acute infection
chronic_sensitivity = 0.98  # The sensitivity for background antibody screening with chronic infection
specificity         = 0.98  # The specificity for background antibody screening
```

#### screening_background_rna

The screening_background_rna section governs the details around background RNA hepatitis-C screening. An example is given below:

```ini
[screening_background_rna]
cost                = 80.38 # The cost for applying a background RNA screen
acute_sensitivity   = 0.988 # The sensitivity for background RNA screening with acute infection
chronic_sensitivity = 0.988 # The sensitivity for background RNA screening with chronic infection
specificity         = 1.0   # The specificity for background RNA screening
```

#### screening_intervention_ab

The screening_intervention_ab section governs the details around intervention antibody hepatitis-C screening. An example is given below:

```ini
[screening_intervention_ab]
cost                = 36.56 # The cost for applying a intervention antibody screen
acute_sensitivity   = 0.98  # The sensitivity for intervention antibody screening with acute infection
chronic_sensitivity = 0.98  # The sensitivity for intervention antibody screening with chronic infection
specificity         = 0.98  # The specificity for intervention antibody screening
```

#### screening_intervention_rna

The screening_intervention_rna section governs the details around intervention RNA hepatitis-C screening. An example is given below:

```ini
[screening_intervention_rna]
cost                = 80.38 # The cost for applying a intervention RNA screen
acute_sensitivity   = 0.988 # The sensitivity for intervention RNA screening with acute infection
chronic_sensitivity = 0.988 # The sensitivity for intervention RNA screening with chronic infection
specificity         = 1.0   # The specificity for intervention RNA screening
```

#### linking

The linking section governs the details around linking to care for a hepatitis-C infection. An example is given below:

```ini
[linking]
intervention_cost               = 0             # Cost for linking due to an intervention screen
voluntary_relinkage_probability = 0.020580315   # Voluntary relinking monthly probability
voluntary_relink_duration       = 16            # Time period in months (since unlinking from treatment) during which a person can relink
false_positive_test_cost        = 515.79        # Cost of a false positive test
recent_screen_multiplier        = 1.0           # 
recent_screen_cutoff            = 0
```

#### treatment

The treatment section governs the details around hepatitis-C treatment. An example is given below:

```ini
[treatment]
treatment_cost      = 118.72    # The monthly cost of applying a treatment
salvage_cost        = 131.81    # The monthly cost of applying a salvage treatment
treatment_utility   = 0.99      # The monthly utility of a person on treatment
treatment_limit     = 100       # The maximum number of treatments a person can go through
tox_cost            = 201.28    # The cost of a toxic reaction in treatment
tox_utility         = 0.21      # The utility of a toxic reaction in treatment
```

#### cost

The cost section governs the details around general costs in the model. An example is given below:

```ini
[cost]
discounting_rate = 0.0025 # annual rate, defaults to 3%
```

### Input Database

All tabular data is stored in the database. In order to generate the database, one simply needs to execute the [script in the examples](extras/examples/inputs.db.sql) and the SQLite file is built accordingly. Due to data constraints, we are not able to share the data we used with our model, only the schema. Users have to insert their own data accordingly.

#### antibody_testing

TODO

#### background_impacts

TODO

#### background_mortality

TODO

#### behavior_impacts

TODO

#### behavior_transitions

TODO

#### bool_lookup

TODO

#### drug_behaviors

TODO

#### fibrosis

TODO

#### fibrosis_diagnosis_states

TODO

#### fibrosis_states

TODO

#### hcv_impacts

TODO

#### incidence

TODO

#### init_cohort

TODO

#### link_states

TODO

#### lost_to_follow_up

TODO

#### moud

TODO

#### pregnancy

TODO

#### pregnancy_states

TODO

#### screening_and_linkage

TODO

#### sex

TODO

#### smr

TODO

#### treatment_initiations

TODO

#### treatments

TODO

Previous: [Installation](installation.md)

Next: [Events](events.md)
