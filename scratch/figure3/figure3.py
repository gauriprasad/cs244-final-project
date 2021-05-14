import os
import matplotlib.pyplot as plt

# Plot the Estimated BW over Time
algorithms = ["TcpWestwood", "TcpWestwoodPlus"]
for a in algorithms:
    dir = 'outputs/figure3/' + a
    estimatedBWTraceTimes = []
    estimatedBWTraceVals = []

    with open(os.path.join(dir, 'estimated-bw.tr'),'r') as f:
        for line in f:
            estimatedBWTrace = line.split()

            estimatedBWTraceTimes.append(float(estimatedBWTrace[0]))
            estimatedBWTraceVals.append((8 * float(estimatedBWTrace[1])) / (1000 * 1000))

    estimatedBWFileName = os.path.join(dir, 'figure3.png')

    plt.figure()
    plt.plot(estimatedBWTraceTimes, estimatedBWTraceVals, label="BW estimate")
    plt.step([0, 25, 50, 75, 125, 175, 200, 300], [0, 5, 4, 3, 4, 3, 4, 5], label="actual BW")
    plt.ylabel('Bandwidth estimation (Mb/sec)')
    plt.xlabel('Time (sec)')
    plt.title('Figure 3: TCPW with concurrent UDP traffic: bandwidth estimation')
    plt.grid()
    plt.legend()
    plt.savefig(estimatedBWFileName)
    print('Saving ' + estimatedBWFileName)