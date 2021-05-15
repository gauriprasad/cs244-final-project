import argparse
import os
import matplotlib.pyplot as plt
import numpy as np
import sys

# Plot the bandwidth

linktype = "lossy"

outputs_dir = 'outputs/figure7/' + linktype + 'link'
fileName = os.path.join(outputs_dir,'friendliness.png')

# Read in trace values

def read_trace(arrTimes, arrVals, trFile):
    with open(os.path.join(outputs_dir, trFile),'r') as f:
        for line in f:
            trace = line.split()
            arrTimes.append(float(trace[0]))
            arrVals.append(float(trace[1]) / float(trace[0]) / 125000) # bandwidth = seqno / time

reno_averages = []
westwood_averages = []

for numReno in range(0, 6):

    # Get average of westwood runs
    westwood_throughputs = []
    for seqnumi in range(0, 5 - numReno):
        times = []
        vals = []
        read_trace(times, vals, str(numReno) + 'reno_seqnums' + str(seqnumi) + '.tr')
        westwood_throughputs.append(vals[-1])

    reno_throughputs = []
    for seqnumi in  range(5 - numReno, 5):
        times = []
        vals = []
        read_trace(times, vals, str(numReno) + 'reno_seqnums' + str(seqnumi) + '.tr')
        reno_throughputs.append(vals[-1])

    print(westwood_throughputs, reno_throughputs)

    if len(reno_throughputs) > 0:
        reno_averages.append(sum(reno_throughputs) / len(reno_throughputs))
    if len(westwood_throughputs) > 0:
        westwood_averages.append(sum(westwood_throughputs) / len(westwood_throughputs))

print(reno_averages, westwood_averages)

westwood_x = [0, 1, 2, 3, 4]
reno_x = [1, 2, 3, 4, 5]

# Plot

plt.figure()
plt.gca().set_ylim(bottom=0, top=2.5)
plt.plot(westwood_x, westwood_averages, c='C0', label='Westwood')
plt.plot(reno_x, reno_averages, c='C1', label='NewReno')
plt.legend(loc='lower right')
plt.ylabel('Average throughput (Mb/Sec)')
plt.xlabel('No. of Reno connections')
plt.title('Over ' + linktype + ' link')
plt.grid()
plt.savefig(fileName)
print('Saving ' + fileName)

