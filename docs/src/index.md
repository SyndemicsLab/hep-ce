# Hepatitis-C Cost Effectiveness

**Source Code:** https://github.com/SyndemicsLab/hep-ce

<a href="https://www.syndemicslab.org/hep-ce">
<img align="right" src="HEPCE_logo.png" alt="HEPCE Logo" height="120" />
</a>

`HEP-CE` is a simulation model written and maintained by the
[Syndemics Lab][syndemics] at [Boston Medical Center][bmc]. `HEP-CE` is a Markov
chain Monte Carlo state-transition model which simulates the spread and
treatment of Hepatitis C Virus (HCV).

The version of `HEP-CE` you see now is a complete refactor developed with a
focus on four goals:

1. Implement the model as a discrete-event simulation
2. Improve the maintainability of the model
3. Make adding/removing events easier
4. Make building the model easier across systems

The [old `HEP-CE` model](https://github.com/SyndemicsLab/hep-ce-v1) (HEPCEv1)
was written in C++, like this one, but to honor the above goals we did not reuse
code from HEPCEv1.

## Key Features

- Easy addition and removal of events
- Parallel execution using OpenMP
- Able to be used standalone or as an API in your projects

## Installation

Clone the source code and then build:
```bash
git clone https://github.com/SyndemicsLab/hep-ce.git
cd hep-ce
cmake --workflow gcc-release
```

## Using the HEP-CE Executable

To use the provided executable, you need data in the correct structure.

Each set of model inputs must be provided in a folder named `input<number>`,
where the `<number>` is replaced with the identifier of the input. The
following depicts a tree of directories in which there are two input sets,
numbered 1 and 2, for `HEP-CE` in the same source directory:

```
data-source/
├── input1/
│   ├── inputs.db
│   └── sim.conf
└── input2/
    ├── inputs.db
    └── sim.conf
```

### Configuration

The `sim.conf` uses the `.ini` configuration format to break down model settings
into key-value pairs that are also grouped into sections. For detailed
descriptions of these values, reference [Managing the Data](data.md).

### Database Schema

An empty database with the correct structure can be created via the
[example schema][dbscheme].

### Calling the Executable

Once you have model inputs in the correct structure, you can call the executable
from the git directory with

```bash
build/extras/executable/hepce_exe /path/to/data/source <input_number_start> <input_number_end>
```

#### Example

To run the model for the example `data-source` shown above, you'd use the
following command:
```bash
build/extras/executable/hepce_exe data-source 1 2
```

<div class="section_buttons">

| Previous |                               Next |
|:---------|-----------------------------------:|
|          | [Building & Installation][install] |

</div>

[bmc]: https://bmc.org
[dbscheme]: https://github.com/SyndemicsLab/hep-ce/tree/main/extras/examples/inputs.db.sql
[install]: installation.md
[syndemics]: https://www.syndemicslab.org
