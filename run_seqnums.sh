#!/bin/bash

./waf --run "scratch/seqnums/seqnums --transport_prot=TcpReno"
./waf --run "scratch/seqnums/seqnums --transport_prot=TcpWestwood"
python3 scratch/seqnums/seqnums.py
python3 -m http.server
