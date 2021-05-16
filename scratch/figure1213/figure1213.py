import os
import matplotlib.pyplot as plt


algorithms = ["TcpNewReno", "TcpVegas", "TcpWestwood", "TcpWestwoodPlus"]
for a in algorithms:
    dir = "outputs/figure1213/" + a
    cwndTraceTimes = []
    cwndTraceVals = []

    ssThreshTraceTimes = []
    ssThreshTraceVals = []

    with open(os.path.join(dir, 'cwnd.tr'),'r') as f:
        for line in f:
            cwndTrace = line.split()

            cwndTraceTimes.append(float(cwndTrace[0]))
            cwndTraceVals.append(float(cwndTrace[1]) / 400)

    with open(os.path.join(dir, 'ssthresh.tr'),'r') as f:
        for line in f:
            ssThreshTrace = line.split()

            ssThreshTraceTimes.append(float(ssThreshTrace[0]))
            ssThreshTraceVals.append(float(ssThreshTrace[1]) / 400)

    figure12FileName = os.path.join(dir, 'figure1213.png')

    plt.figure()
    plt.plot(ssThreshTraceTimes[10:], ssThreshTraceVals[10:], label="ssthresh")
    plt.plot(cwndTraceTimes[10:], cwndTraceVals[10:], "--", label="congestion window")
    plt.ylabel('Segments')
    plt.ylim([0, 100])
    plt.xlabel('Time (s)')
    plt.grid()
    plt.legend()
    plt.savefig(figure12FileName)
    print('Saving ' + figure12FileName)
