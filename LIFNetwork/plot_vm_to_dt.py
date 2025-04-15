import sys
import numpy as np
import matplotlib.pyplot as plt

def read_values(filename):
    with open(filename, 'r') as file:
        values = [float(line.strip()) for line in file]
    return values

def plot_values_over_time(current_values, vm_values, vout_values, dt=0.01, duration=50):
    time = np.arange(0, len(current_values) * dt, dt)
    
    # Limit the time and values to the specified duration
    limit = int(duration / dt)
    time = time[:limit]
    current_values = current_values[:limit]
    vm_values = vm_values[:limit]
    vout_values = vout_values[:limit]
    
    plt.figure(figsize=(12, 8))
    
    plt.subplot(3, 1, 1)
    plt.plot(time, current_values)
    plt.xlabel('Time (s)')
    plt.ylabel('Current (I_in)')
    plt.title('Input Current Over Time (First 2 Seconds)')
    plt.grid(True)
    
    plt.subplot(3, 1, 2)
    plt.plot(time, vm_values)
    plt.xlabel('Time (s)')
    plt.ylabel('Membrane Potential (Vm)')
    plt.title('Membrane Potential Over Time (First 2 Seconds)')
    plt.grid(True)
    
    plt.subplot(3, 1, 3)
    plt.plot(time, vout_values)
    plt.xlabel('Time (s)')
    plt.ylabel('Output Voltage (Vout)')
    plt.title('Output Voltage Over Time (First 2 Seconds)')
    plt.grid(True)
    
    plt.tight_layout()
    plt.show()

if __name__ == '__main__':
    if len(sys.argv) < 4:
        print("Usage: python script.py <Iin_filename> <Vm_filename> <Vout_filename>")
        sys.exit(1)

    Iin_filename = sys.argv[1]
    Vm_filename = sys.argv[2]
    Vout_filename = sys.argv[3]
    
    current_values = read_values(Iin_filename)
    vm_values = read_values(Vm_filename)
    vout_values = read_values(Vout_filename)
    
    plot_values_over_time(current_values, vm_values, vout_values)