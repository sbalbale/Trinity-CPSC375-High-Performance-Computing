#!/bin/bash
echo "=== 1. Testing Passwordless SSH Management Network ==="
for node in node01 node02 node03; do
    echo -n "Checking $node: "
    ssh $node hostname
done

echo -e "\n=== 2. Testing Simultaneous Execution via Slurm ==="
srun -p compute -N 3 hostname
