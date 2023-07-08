#!/bin/bash

# set -o errexit
set -o nounset

exedir="$(dirname $0)/.."

print_usage() {
    echo "usage: $0 [--valgrind]"
}

if [[ $# > 1 ]]; then
    print_usage
    exit 1
fi

valgrind=
if [[ $# > 0 ]]; then
    if [[ "$1" == "--valgrind" ]]; then
        valgrind="valgrind -q"
        shift 1
    else
        print_usage
        exit 1
    fi
fi

# main programs
opt_md="$valgrind $exedir"/opt-md
opt_sd="$valgrind $exedir"/opt-sd
opt_sd_gsl="$valgrind $exedir"/opt-sd-gsl
# helper programs
eval_dGdp="$valgrind $exedir"/eval-dGdp
eval_pseq="$valgrind $exedir"/eval-pseq
eval_score="$valgrind $exedir"/eval-score
eval_useq="$valgrind $exedir"/eval-useq
random_seq="$valgrind $exedir"/random-seq
random_vienna="$valgrind $exedir"/random-vienna

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
