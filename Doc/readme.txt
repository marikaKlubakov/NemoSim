# NEMOSIM  
  
## Overview  
  
NEMOSIM.exe is a simulation tool that uses two input files: an XML configuration file and a TXT file containing current input values.  
  
## Usage  

#LIF simulation :
  
1. To run the LIF network simulation, use the following command:  
  
NEMOSIM.exe .\Tests\SNN\LIF\sin_current_test\test.xml .\Tests\SNN\LIF\sin_current_test\input.txt  
  
### Input Files  
  
- XML Configuration File: Specifies the configuration parameters for the simulation.  
- TXT Input File: Contains the current input values.  
  
### Output Files  
  
The simulation generates three output files:  
  
- Iins.txt: Shows the current input values.  
- vms.txt: Contains the membrane potential values.  
- Vouts.txt: Displays the output voltage (spikes).  
  
2. After the simulation, run the following Python script to plot the values over time:  
  
python plot_vm_to_dt.py Iins.txt vms.txt Vouts.txt  
  
This will generate an image showing the values over time.  
  
## Generating New Input Files  
  
To create new current input values, use the input_creator.py script. Run the script and provide a formula representing the new input current with t as the variable. For example:  
  
python input_creator.py   
> "1e-10 * math.sin(2 * math.pi * t * 5 + 3 * math.pi / 2) + 1e-10"


#BIU simulation :
  
1. To run the BIU network simulation, use the following command:  
  
NEMOSIM.exe .\Tests\SNN\BIU\test.xml .\Tests\SNN\BIU\input.txt <.\Tests\SNN\BIU\supervisor.xml>
  
### Input Files  
  
- XML Configuration File: Specifies the configuration parameters and architecture for the simulation.  
- TXT Input File: Contains the current input values.  
- XML Supervisor Configuration File <optional>: Specifies the configuration parameters for the simulation.
### Output Files  
  
The simulation generates three output files:  
  
- Vin.txt: Shows the synapse input values.  
- Vns.txt: Contains the Vn potential values.  
- Spikes.txt: Displays the output voltage (spikes).  
  
2. After the simulation, run the following Python script to plot the values over time:  
  
python plot_vn_to_dt.py Vns.txt Spikes.txt Vin.txt  
  
This will generate an image showing the values over time.  