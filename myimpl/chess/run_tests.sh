#!/bin/bash
# Simple test harness for the Position Analyzer
# Each case: commands to set up board then eval

echo "Running chess analyzer testcases..."

run_case() {
    local desc="$1"
    shift
    echo "$desc"
    # %b interprets \n sequences
    printf "%b" "$*" | ./a.out
    echo
}

# compilation check
make || exit 1

# empty board
run_case "Empty board should report nothing" "clear\neval 1\nquit\n"

# mate-in-1 example
run_case "Mate in one (Qxa8)" "clear\nset a1 K\nset a5 Q\nset a8 r\nset h8 k\neval 4\nquit\n"

# smothered mate motif
run_case "Smothered mate pattern" "clear\nset h1 K\nset h6 Q\nset e5 N\nset h8 k\nset a8 r\neval 5\nquit\n"

# illegal king capture prevention
run_case "Illegal king-capture not generated" "clear\nset h1 K\nset g8 Q\nset h8 k\nset a8 r\neval 4\nquit\n"

echo "Tests complete."
