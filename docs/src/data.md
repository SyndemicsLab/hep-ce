# Inputs & Outputs

The `HEP-CE` model requires structured data in order to obtain meaningful
outcomes. Here, we'll describe, in detail, how your data should be structured
when using the model.

## Directory Structure

The example, reiterated below, from the [landing page](index.md) illustrates
some details about the necessary file structure to use the model:

```
data-source/
├── input1/
│   ├── inputs.db
│   └── sim.conf
└── input2/
    ├── inputs.db
    └── sim.conf
```

Let's break all this down. First, there's the `data-source/` directory,
containing individual input sets for the model. While the executable doesn't
require a folder analogous to `data-source`, it can be useful to use such
a folder as a record of what the inputs within it represent (e.g. `basecase/`,
`hcv-positive-cohort/`, etc.) since you can name these folders arbitrarily.
Regardless of whether or not you use such a directory, there must be at least
one input folder, named `input<X>` where `<X>` is replaced with an integer, that
contains the two required files `sim.conf` and `inputs.db`, which will be
explained in detail in the following sections.

**Note:** There is no requirement that input folders start at 1. You can have
any assortment of input numbers, though it is convenient for their numbers to be
sequential, as using the executable with multiple input sets in one call expects
them to be sequential. Consider the situation where you have the following
input folders in `data-source/`:

```
input7, input8, input10, input11
```

Because they are not sequential, you would need to use two calls to the
executable to simulate all of the inputs:

```
build/extras/executable/hepce_exe data-source 7 8
build/extras/executable/hepce_exe data-source 10 11
```

Our recommendation is simply that you structure your inputs in a way that makes
them intuitive and interpretable, so that when someone looks at the data, they
might understand the meaning without significant effort.

## Input Files

Within each numbered input folder, there are two files, `sim.conf` and
`inputs.db`, which are both required to use the `HEP-CE` executable. `sim.conf`
is a standard, [`.ini`-formatted][ini] configuration file and `inputs.db` is a
[SQLite database][sqlite]. In the next sections, we'll dive into the details of
each of these files.

### sim.conf

The `sim.conf` file defines the structure of a `HEP-CE` simulation run. In this
file the discrete events, characteristics of HCV care, and single-value
parameters are specified—there is no tabular data in this file. As is typical of
`.ini` files, `sim.conf` is broken down into sections, each of which contain
key-value pairs. `HEP-CE` has fourteen such sections:

- `simulation`
- `mortality`
- `infection`
- `eligibility`
- `fibrosis`
- `fibrosis_staging`
- `screening`
- `screening_background_ab`
- `screening_background_rna`
- `screening_intervention_ab`
- `screening_intervention_rna`
- `linking`
- `treatment`
- `cost`

[An example `sim.conf`][exampleconf] is provided within the GitHub repository
for reference.

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
treatment_limit     = 100       # The maximum number of treatments a person can experience in the simulation
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

## Outputs

TODO

<div class="section_buttons">

| Previous                |             Next |
|:------------------------|-----------------:|
| [Installation][install] | [Events][events] |

</div>

[ini]: https://docs.fileformat.com/system/ini/
[install]: installation.md
[events]: events.md
[exampleconf]: https://github.com/SyndemicsLab/hep-ce/blob/main/extras/examples/sim.conf
[sqlite]: https://sqlite.org/index.html
