#! /bin/bash

# This script is used to kill all crab jobs in the current directory

for i in `ls -d crab_*`; do
    echo "Killing $i"
    crab kill -d $i &
done
