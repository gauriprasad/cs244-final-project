import argparse
import os
import matplotlib.pyplot as plt
import numpy as np
import sys

# Plot the sequence numbers over Time

outputs_dir = 'outputs/figure45'
fileName = os.path.join(outputs_dir,'seqnums.png')

# Read in trace values

def read_trace(arrTimes, arrVals, trFile):
    with open(os.path.join(outputs_dir, trFile),'r') as f:
        for line in f:
            trace = line.split()
            arrTimes.append(float(trace[0]))
            arrVals.append(float(trace[1]) / (400-42) / 100)

renoFastTraceTimes = []
renoFastTraceVals = []
read_trace(renoFastTraceTimes, renoFastTraceVals, 'TcpRenoSeqnumFast.tr')

renoSlowTraceTimes = []
renoSlowTraceVals = []
read_trace(renoSlowTraceTimes, renoSlowTraceVals, 'TcpRenoSeqnumSlow.tr')

westwoodFastTraceTimes = []
westwoodFastTraceVals = []
read_trace(westwoodFastTraceTimes, westwoodFastTraceVals, 'TcpWestwoodSeqnumFast.tr')

westwoodSlowTraceTimes = []
westwoodSlowTraceVals = []
read_trace(westwoodSlowTraceTimes, westwoodSlowTraceVals, 'TcpWestwoodSeqnumSlow.tr')

# Plot

plt.figure()
plt.plot(renoFastTraceTimes, renoFastTraceVals, c="C1", label="NewReno")
plt.plot(renoSlowTraceTimes, renoSlowTraceVals, c="C1")
plt.plot(westwoodFastTraceTimes, westwoodFastTraceVals, c="C0", label="Westwood")
plt.plot(westwoodSlowTraceTimes, westwoodSlowTraceVals, c="C0")
plt.legend(loc='upper left')
plt.ylabel('Sequence number')
plt.xlabel('Seconds')
plt.grid()
plt.savefig(fileName)
print('Saving ' + fileName)

