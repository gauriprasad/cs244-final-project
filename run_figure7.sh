#!/bin/bash
  
for linkType in "good" "lossy"; do
  for numReno in 0 1 2 3 4 5; do
    ./waf --run "scratch/figure7/figure7 --numReno=$numReno --linkType=$linkType"
    python3 scratch/figure7/figure7.py $numReno $linkType
  done
  python3 scratch/figure7/figure7_final.py $linkType
done

