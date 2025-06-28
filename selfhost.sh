#!/bin/bash
set -e

# Compile each C file in the repository using the existing 9cc
for c in *.c; do
    ./9cc "$c" > "${c%.c}.s"
done

# Assemble assembly files
for s in *.s; do
    cc -c "$s"
done

# Link object files into a self-hosted compiler
cc -o 9cc-self *.o

echo "Self-hosted compiler generated as ./9cc-self"
