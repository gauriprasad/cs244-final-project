#!/bin/bash
  
for numReno in 1 2 3 4 5 0; do
  ./waf --run "scratch/figure7/figure7 --numReno=$numReno"
  python3 scratch/figure7/figure7.py $numReno
done

#python3 -m http.server
