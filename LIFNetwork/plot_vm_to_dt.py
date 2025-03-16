import sys  
import numpy as np  
import matplotlib.pyplot as plt  
  
def read_vm_values(filename):  
    with open(filename, 'r') as file:  
        vm_values = [float(line.strip()) for line in file]  
    return vm_values  
  
def downsample(values, factor):  
    return values[::factor]  
  
def plot_vm_over_time(vm_values, dt=0.0000001, downsample_factor=100):  
    downsampled_values = downsample(vm_values, downsample_factor)  
    time = np.arange(0, len(vm_values) * dt, dt * downsample_factor)  
    plt.plot(time, downsampled_values)  
    plt.xlabel('Time (s)')  
    plt.ylabel('Membrane Potential (Vm)')  
    plt.title('Membrane Potential Over Time (Downsampled)')  
    plt.grid(True)  
    plt.show()  
  
if __name__ == '__main__':  
    if len(sys.argv) < 2:  
        print("Usage: python script.py <filename>")  
        sys.exit(1)  
  
    filename = sys.argv[1]  
    vm_values = read_vm_values(filename)  
    plot_vm_over_time(vm_values)  