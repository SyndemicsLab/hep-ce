# Hepatitis-C Cost Effectiveness

**Source Code:** https://github.com/SyndemicsLab/hep-ce

<a href="https://www.syndemicslab.org/hep-ce"><img align="right" src="HEPCE_logo.png" alt="HEPCE Logo" height="120" /></a>

`HEP-CE` is a simulation model written and maintained by the [Syndemics Lab](https://www.syndemicslab.org) at [Boston Medical Center](https://bmc.org).
This is the `HEP-CE` user guide.

The version of `HEP-CE` you see now is a complete refactor developed with a focus on four goals:

1. Implement the model as a discrete-event simulation
2. Improve the maintainability of the model
3. Make adding/removing events easier
4. Make building the model easier across systems

The [old `HEP-CE` model](https://github.com/SyndemicsLab/hep-ce-v1) (HEPCEv1) was written in C++, like this one, but to honor the above goals we did not reuse code from HEPCEv1.

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

To use the executable, you need to structure your data correctly. config file and a SQLite database.

Each run is put into a folder titled `input<number>` where the number is replaced with the ID of the input. Then, after these folders are created and HEPCE is built we simply run the command:

### Database Schema

An empty database with the correct structure can be created via the [example schema](https://github.com/SyndemicsLab/hep-ce/tree/main/extras/examples/inputs.db.sql).

### Calling the Executable

```bash
build/extras/executable/hepce_exe <input_start> <input_end>
```

Next: [Building & Installation](installation.md)
