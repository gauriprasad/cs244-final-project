import argparse
import os
import matplotlib.pyplot as plt
import numpy as np
import sys

# Plot the bandwidth

numReno = int(sys.argv[1])

outputs_dir = 'outputs/figure7'
fileName = os.path.join(outputs_dir,'friendliness_' + str(5 - numReno) + 'ww' + str(numReno) + 'nr_5Mb.png')

# Read in trace values

def read_trace(arrTimes, arrVals, trFile):
    with open(os.path.join(outputs_dir, trFile),'r') as f:
        for line in f:
            trace = line.split()
            arrTimes.append(float(trace[0]))
            arrVals.append(float(trace[1]) / float(trace[0]) / 125000) # bandwidth = seqno / time

times0 = []
vals0 = []
read_trace(times0, vals0, str(numReno) + 'reno_seqnums0.tr')

times1 = []
vals1 = []
read_trace(times1, vals1, str(numReno) + 'reno_seqnums1.tr')

times2 = []
vals2 = []
read_trace(times2, vals2, str(numReno) + 'reno_seqnums2.tr')

times3 = []
vals3 = []
read_trace(times3, vals3, str(numReno) + 'reno_seqnums3.tr')

times4 = []
vals4 = []
read_trace(times4, vals4, str(numReno) + 'reno_seqnums4.tr')

# Plot

c_s = []
label_s = []
for i in range(5 - numReno):
    c_s += ["C0"]
    label_s += ["Westwood"]
for i in range(numReno):
    c_s += ["C1"]
    label_s += ["NewReno"]

plt.figure()
plt.plot(times0, vals0, c=c_s[0], label=label_s[0])
plt.plot(times1, vals1, c=c_s[1], label=label_s[1])
plt.plot(times2, vals2, c=c_s[2], label=label_s[2])
plt.plot(times3, vals3, c=c_s[3], label=label_s[3])
plt.plot(times4, vals4, c=c_s[4], label=label_s[4])
plt.legend(loc='upper left')
plt.ylabel('Average Throughput (Mb/Sec)')
plt.xlabel('Seconds')
plt.grid()
plt.savefig(fileName)
print('Saving ' + fileName)

