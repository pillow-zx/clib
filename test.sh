#!/usr/bin/bash

for test in build/test/*; do 
  echo "=== Running $(basename $test) ===" && $test; 
done
