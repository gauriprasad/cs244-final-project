#!/bin/bash

for PROTOCOL in "TcpWestwood" "TcpWestwoodPlus"; do
  DIR=outputs/figure3/$PROTOCOL
  [ ! -d $DIR ] && mkdir $DIR

  # Run the NS-3 Simulation
  ./waf --run "figure3 --transport_prot=$PROTOCOL"
done

# Plot the trace figures
python3 scratch/figure3/figure3.py