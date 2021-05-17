import argparse
import os
import matplotlib.pyplot as plt
import numpy as np

# Plot the Throughput over Delay

dir = "outputs/figure10/"
algorithms = ["TcpNewReno", "TcpSack", "TcpWestwood", "TcpWestwoodPlus", "TcpVegas"]
delays = [1, 2.5, 5, 10, 25, 50, 100, 250]

plt.figure()
for a in algorithms:
    throughput = []
    algorithm_dir = dir + a
    for d in delays:
        file_name = str(d) + "msthroughput.tr"
        with open(os.path.join(algorithm_dir, file_name),'r') as f:
            for line in f:
                throughputTrace = line.split()
                throughput.append(float(throughputTrace[1]))
    plt.plot(delays, throughput, "x--", label=a)

throughputFileName = os.path.join(dir, "figure10.png")
plt.ylabel('Throughput (Mb/sec)')
plt.xlabel('One Way Propagation Time (msec)')
plt.grid()
plt.legend()
plt.savefig(throughputFileName)
print('Saving ' + throughputFileName)
