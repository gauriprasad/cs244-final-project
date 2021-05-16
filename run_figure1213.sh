#!/bin/bash

for PROTOCOL in "TcpNewReno" "TcpVegas" "TcpWestwood" "TcpWestwoodPlus"; do
  DIR=outputs/figure1213/$PROTOCOL
  [ ! -d $DIR ] && mkdir $DIR

  # Run the NS-3 Simulation
  ./waf --run "figure1213 --transport_prot=$PROTOCOL"
done

# Plot the trace figures