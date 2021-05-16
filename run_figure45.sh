#!/bin/bash

./waf --run "scratch/figure45/figure45 --transport_prot=TcpReno"
./waf --run "scratch/figure45/figure45 --transport_prot=TcpWestwood"
python3 scratch/figure45/figure45.py

