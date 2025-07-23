# NemoSim

This repository contains the Nemo Project simulator implementation.

---

## Project Overview

**NemoSim** is a simulation tool for spiking neural network architectures, developed as part of the Nemo Project. It enables researchers and engineers to model, simulate, and analyze the behavior of various neural network types, such as Leaky Integrate-and-Fire (LIF) and Brain-Inspired Unit (BIU) networks.

### Purpose

NemoSim is designed to:

- **Simulate neural network architectures** using user-defined configurations.
- **Model time-evolving neuron and synapse states** in response to input currents.
- **Support both standard and custom network types** (LIF, BIU, and extensible for new models).
- **Generate outputs for scientific analysis and visualization,** including detailed time-series data for neural state, synapse activity, and spikes.
- **Facilitate debugging and research,** with outputs and tools to identify and analyze network behavior.

### Key Features

- **Flexible Input:** Accepts XML-based network descriptions and plain text current input files, supporting both manual and scripted generation.
- **Extensible Design:** Modular codebase allows for easy addition of new neuron/synapse models and architectures.
- **Comprehensive Output:** Produces multiple output files (membrane potentials, synaptic inputs, spikes, etc.) for analysis and plotting.
- **Analysis Tools:** Includes Python scripts and guidelines for visualizing and interpreting simulation results.
- **Robust Error Handling:** Detects and reports errors in input files and configuration, aiding reproducibility.

### Typical Use Cases

- Computational neuroscience research
- Neuromorphic hardware prototyping and validation
- Algorithm development and prototyping for spiking neural networks

---

## How to Run NemoSim

### 1. Prepare Your Input Files

#### XML Configuration

- Define your network architecture, neuron parameters, and simulation settings in an XML configuration file.
- Example (LIF network):

    ```xml
    <NetworkConfig type="LIF">
      <LIFNetwork>
        <Cm>1.0</Cm>
        <Cf>0.5</Cf>
        <VDD>2.5</VDD>
        <!-- other LIF parameters -->
      </LIFNetwork>
      <Architecture>
        <!-- network connectivity, layers, weights, etc. -->
      </Architecture>
    </NetworkConfig>
    ```

- Example (BIU network):

    ```xml
    <NetworkConfig type="BIU">
      <BIUNetwork>
        <!-- BIU parameters here -->
      </BIUNetwork>
      <Architecture>
        <!-- architecture details -->
      </Architecture>
    </NetworkConfig>
    ```

#### TXT Input File

- Each simulation requires a plain text file with one value per line, representing the input current for each time step (or input channel).
- Example:

    ```
    1
    0
    0
    0
    1
    ...
    ```

- You can generate input files using the provided Python script:

    ```sh
    python input_creator.py
    > "1e-10 * math.sin(2 * math.pi * t * 5 + 3 * math.pi / 2) + 1e-10"
    ```

---

### 2. Run the Simulator

- Basic command (from the root directory):

    ```sh
    NEMOSIM.exe path/to/config.xml path/to/input.txt
    ```

    For example:

    ```sh
    NEMOSIM.exe .\Tests\SNN\LIF\sin_current_test\test.xml .\Tests\SNN\LIF\sin_current_test\input.txt
    ```

- The simulator produces output files in the current directory.

---

### 3. Analyze Outputs

- Output files are plain text, each containing a list of numeric values (one per line).
- Typical output files:
    - **LIF Simulation:**
        - `Iins.txt`: Input currents
        - `vms.txt`: Membrane potentials
        - `Vouts.txt`: Output voltages (spikes)
    - **BIU Simulation:**
        - `Vin.txt`: Synapse input values
        - `Vns.txt`: Neural state potentials
        - `Spikes.txt`: Output spikes

- To plot outputs, use the provided Python scripts (e.g., `plot_vm_to_dt.py`):

    ```sh
    python plot_vm_to_dt.py Iins.txt vms.txt Vouts.txt
    ```

---

## Examples

### Example 1: Running a LIF Simulation

**Files:**

- `test.xml` (XML configuration)
- `input.txt` (input current)

**Command:**

```sh
NEMOSIM.exe .\Tests\SNN\LIF\sin_current_test\test.xml .\Tests\SNN\LIF\sin_current_test\input.txt
```

**Output:**

- `Iins.txt`
- `vms.txt`
- `Vouts.txt`

**Plotting:**

```sh
python plot_vm_to_dt.py Iins.txt vms.txt Vouts.txt
```

### Example 2: Generating Input File with Script

```sh
python input_creator.py
> "1e-10 * math.sin(2 * math.pi * t * 5 + 3 * math.pi / 2) + 1e-10"
```

---

## Data Input File Requirements

### Expected File Format and Structure

- **TXT Input File:**  
  - Each simulation requires a TXT file containing current input values.
  - The file is a plain text file with one value per line.
  - For BIU and LIF network simulations, the values typically represent neuron input currents over time.

### Column and Row Specifications

- **Rows:** Each line represents a time step or input channel.  
- **Columns:** The files are generally single-column, each line containing a single numeric value (no delimiters).
- **Limits:** Number of lines should match the expected simulation time steps.

### Data Type Requirements

- Each value must be numeric (integer or floating-point).

---

## XML Configuration Schema

- The XML file must have a `<NetworkConfig>` root with a `type` attribute.
- Required child elements depend on the network type (`LIF`, `BIU`, ...).
- All required numeric fields must be present and valid numbers.

---

## Output Protocol and Error Handling

### Standard Output Format

- Output files are plain text, each containing a list of numeric values (one per line).

### Error and Warning Message Formats

- Errors (examples):

    ```
    Error loading XML file: <description>
    Error: No <NetworkConfig> root element found.
    Error: Network type attribute not found in <NetworkConfig>.
    ```

- Warnings:

    ```
    Warning: <Cm> missing or invalid in LIFNetwork
    ```

### Complete List of Possible Return Codes

- The code uses TinyXML2, which defines error codes such as:
    - `XML_SUCCESS` (0): Success
    - `XML_NO_ATTRIBUTE`
    - `XML_WRONG_ATTRIBUTE_TYPE`
    - `XML_ERROR_FILE_NOT_FOUND`
    - `XML_ERROR_FILE_COULD_NOT_BE_OPENED`
    - `XML_ERROR_FILE_READ_ERROR`
    - `XML_ERROR_PARSING_ELEMENT`
    - ... (see TinyXML2's `XMLError` enum for full list)

---

## Output Analysis and Debugging

- Each output file corresponds to a specific neural variable:
    - `Iins.txt`: Inputs
    - `vms.txt`: Membrane potentials
    - `Vouts.txt`/`Spikes.txt`: Output spikes
    - `Vin.txt`: Synapse input values
    - `Vns.txt`: Neural state potentials

- By plotting or analyzing the output files, neurons with unexpected behaviors (e.g., constant potentials, no spikes) can be identified for further debugging.

---

## References

- See `Tests/SNN/BIU/` for more details and example files.
- Output file examples: `Vin.txt`, `Vns.txt` (in the test folders).

---