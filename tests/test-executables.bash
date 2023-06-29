#!/bin/bash

# set -o errexit
set -o nounset

exedir="$(dirname $0)/.."

# main programs
opt_md="$exedir"/opt-md
opt_sd="$exedir"/opt-sd
opt_sd_gsl="$exedir"/opt-sd-gsl
# helper programs
eval_dGdp="$exedir"/eval-dGdp
eval_pseq="$exedir"/eval-pseq
eval_score="$exedir"/eval-score
eval_useq="$exedir"/eval-useq
random_seq="$exedir"/random-seq
random_vienna="$exedir"/random-vienna

test_command() {
    test_name=$1
    shift
    echo -n "running test: $test_name "
    "$@" &> /dev/null
    exit_code=$?
    if [[ $exit_code == 0 ]]; then
        echo " SUCCESS"
    else
        echo " FAIL"
        echo "    failed to run: $@"
    fi
}

# basic tests
echo "basic tests"
# main programs
test_command opt-md $opt_md '(((...)))'
test_command opt-sd $opt_md '(((...)))'
test_command opt-sd-gsl $opt_md '(((...)))'
# helper programs
test_command eval-dGdp $eval_dGdp '(((...)))' GGGAAACCC GCCAAUGGC 100
test_command eval-pseq $eval_pseq '(((...)))' GGGAAACCC
test_command eval-score $eval_score '(((...)))' GGGAAACCC
test_command eval-useq $eval_useq '(((...)))' GGGAAACCC
test_command random-seq $random_seq '(((...)))'
test_command random-vienna $random_vienna 10 3
