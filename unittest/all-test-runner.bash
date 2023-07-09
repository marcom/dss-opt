#!/bin/bash

set -o nounset
# Not using errexit because it causes issues with the grep commands.
# set -o errexit

cd "$(dirname "$0")"
echo "pwd = $(pwd)"

# We only test the first command-line arg on purpose, so the
# command_prefixes could in theory include "-h" or "--help" options.
if [[ $# > 0 && ("$1" == "-h" || "$1" == "--help") ]]; then
    echo "usage: $0 [command_prefix(es)]"
    echo "examples for command prefixes:"
    echo "    $0 valgrind"
    echo "    $0 valgrind -q"
    exit 0
fi

total_tests=0
total_failures=0
total_ignored=0

for testprog in ./run-*; do
    echo "$testprog"
    out=$($@ "$testprog")
    echo "$out"
    # The `-z` option outputs null bytes, which we need for multi-line
    # grepping, so the `tr` call translates them back.
    result=$(echo "$out" | grep -Pzo '\n\-\-\-\-\-\-+\n\d+ Tests \d+ Failures \d+ Ignored' | tr '\0' '\n')
    echo "result = $result"
    # Count tests and failures, assumes UNITY_OUTPUT_COLOR is not enabled.
    num_tests=$(echo "$result" | grep -oP '\d+(?= Tests)')
    num_failures=$(echo "$result" | grep -oP '\d+(?= Failures)')
    num_ignored=$(echo "$result" | grep -oP '\d+(?= Ignored)')
    ((total_tests+=num_tests))
    ((total_failures+=num_failures))
    ((total_ignored+=num_ignored))
done

echo
echo "################################"
echo "# Unittest results"
echo "# Ran $total_tests tests. $total_failures failed. $total_ignored ignored."
echo "################################"
echo

if [[ "$total_failures" == 0 ]]; then
    exit 0
else
    exit "$total_failures"
fi
