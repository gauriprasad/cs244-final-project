import argparse
import os
import matplotlib.pyplot as plt
import numpy as np

# Plot the sequence numbers over Time

outputs_dir = 'outputs'
fileName = os.path.join(outputs_dir, 'seqnums.png')

# Read in trace values

renoFastTraceTimes = []
renoFastTraceVals = []
with open(os.path.join(outputs_dir, 'TcpRenoSeqnumFast.tr'),'r') as f:
    for line in f:
        trace = line.split()
        renoFastTraceTimes.append(float(trace[0]))
        renoFastTraceVals.append((8 * float(trace[1])) / (1000 * 1000))

renoSlowTraceTimes = []
renoSlowTraceVals = []
with open(os.path.join(outputs_dir, 'TcpRenoSeqnumSlow.tr'),'r') as f:
    for line in f:
        trace = line.split()
        renoSlowTraceTimes.append(float(trace[0]))
        renoSlowTraceVals.append((8 * float(trace[1])) / (1000 * 1000))

westwoodFastTraceTimes = []
westwoodFastTraceVals = []
with open(os.path.join(outputs_dir, 'TcpWestwoodSeqnumFast.tr'),'r') as f:
    for line in f:
        trace = line.split()
        westwoodFastTraceTimes.append(float(trace[0]))
        westwoodFastTraceVals.append((8 * float(trace[1])) / (1000 * 1000))

westwoodSlowTraceTimes = []
westwoodSlowTraceVals = []
with open(os.path.join(outputs_dir, 'TcpWestwoodSeqnumSlow.tr'),'r') as f:
    for line in f:
        trace = line.split()
        westwoodSlowTraceTimes.append(float(trace[0]))
        westwoodSlowTraceVals.append((8 * float(trace[1])) / (1000 * 1000))

# Plot

plt.figure()
plt.plot(renoFastTraceTimes, renoFastTraceVals, c="C1")
plt.plot(renoSlowTraceTimes, renoSlowTraceVals, c="C1")
plt.plot(westwoodFastTraceTimes, westwoodFastTraceVals, c="C0")
plt.plot(westwoodSlowTraceTimes, westwoodSlowTraceVals, c="C0")
plt.ylabel('Sequence number')
plt.xlabel('Seconds')
plt.grid()
plt.savefig(fileName)
print('Saving ' + fileName)
