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
for reference. All key-value pairs for each section are documented in detail
in the example.

### Input Database

All tabular data is stored in the database. In order to generate a database with
the necessary structure, one simply needs to execute the [script][examplesql]
provided in the examples—the SQLite file is built accordingly. Due to data use
agreements, we are not able to share the data we use with our model, only the
schema. Users will have to provide their own data. The tables within `inputs.db`
are named as follows:

- `antibody_testing`
- `background_impacts`
- `background_mortality`
- `behavior_impacts`
- `behavior_transitions`
- `bool_lookup`
- `drug_behaviors`
- `fibrosis`
- `fibrosis_diagnosis_states`
- `fibrosis_states`
- `hcv_impacts`
- `hcv_states`
- `incidence`
- `init_cohort`
- `link_states`
- `lost_to_follow_up`
- `moud`
- `pregnancy`
- `pregnancy_states`
- `screening_and_linkage`
- `sex`
- `smr`
- `treatment_initiations`
- `treatments`

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
[examplesql]: https://github.com/SyndemicsLab/hep-ce/blob/main/extras/examples/inputs.db.sql
[sqlite]: https://sqlite.org/index.html
