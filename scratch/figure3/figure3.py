import argparse
import os
import matplotlib.pyplot as plt
import numpy as np

# Plot the Estimated BW over Time

estimatedBWTraceTimes = []
estimatedBWTraceVals = []

dir = 'outputs/figure3'

with open(os.path.join(dir, 'estimated-bw.tr'),'r') as f:
    for line in f:
        estimatedBWTrace = line.split()

        estimatedBWTraceTimes.append(float(estimatedBWTrace[0]))
        estimatedBWTraceVals.append((8 * float(estimatedBWTrace[1])) / (1000 * 1000))

estimatedBWFileName = os.path.join(dir, 'estimated-bw.png')

plt.figure()
plt.plot(estimatedBWTraceTimes, estimatedBWTraceVals, c="C0")
plt.ylabel('Bandwidth')
plt.xlabel('Seconds')
plt.grid()
plt.savefig(estimatedBWFileName)
print('Saving ' + estimatedBWFileName)