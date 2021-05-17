import os
import matplotlib.pyplot as plt

# Plot the Estimated BW over Time
algorithms = ["TcpWestwood", "TcpWestwoodPlus"]
dir = 'outputs/figure3/'
estimatedBWFileName = os.path.join(dir, 'figure3.png')
plt.figure()
for a in algorithms:
    algorithm_dir = dir + a
    estimatedBWTraceTimes = []
    estimatedBWTraceVals = []

    with open(os.path.join(algorithm_dir, 'estimated-bw.tr'),'r') as f:
        for line in f:
            estimatedBWTrace = line.split()

            estimatedBWTraceTimes.append(float(estimatedBWTrace[0]))
            estimatedBWTraceVals.append((8 * float(estimatedBWTrace[1])) / (1000 * 1000))

    plt.plot(estimatedBWTraceTimes, estimatedBWTraceVals, "x", label="BW estimate")

plt.step([0, 25, 50, 75, 125, 175, 200, 300], [0, 5, 4, 3, 4, 3, 4, 5], label="actual BW")
plt.ylabel('Bandwidth estimation (Mb/sec)')
plt.xlabel('Time (sec)')
plt.title('Figure 3: TCPW with concurrent UDP traffic: bandwidth estimation')
plt.ylim([0, 6])
plt.grid()
plt.legend()
plt.savefig(estimatedBWFileName)
print('Saving ' + estimatedBWFileName)