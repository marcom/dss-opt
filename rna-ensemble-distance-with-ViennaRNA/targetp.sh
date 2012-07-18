#!/bin/sh

RNA_ENS_DIST=$(dirname $0)/rna-ensemble-distance

if [ $# -ne 2 ]; then
    echo "usage: $0 sequence target-struct"
    echo "       The program returns the probability of the target structure"
    echo "       given in Vienna secondary structure notation for the sequence."
    exit 1
fi

seq=$1
target=$2

"$RNA_ENS_DIST" "$seq" "$target" | grep '^target_p' | cut -d '=' -f 2
