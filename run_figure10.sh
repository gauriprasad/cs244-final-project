#!/bin/bash

for DELAY in "1ms" "2.5ms" "5ms" "10ms" "25ms" "50ms" "100ms" "250ms"; do
  for PROTOCOL in "TcpNewReno" "TcpSack" "TcpWestwood" "TcpWestwoodPlus" "TcpVegas"; do
    DIR=outputs/$PROTOCOL
    [ ! -d $DIR ] && mkdir $DIR

    # Run the NS-3 Simulation
    ./waf --run "figure10 --transport_prot=$PROTOCOL --delay_wired=$DELAY"
  done
done