#!/bin/bash

# Če katerikoli ukaz pade, se skripta takoj ustavi
set -e

echo "Prevajam program..."

$CXX -Wall -Wextra -std=c++17 vaja5.cpp -o vaja5

echo "Program se je uspešno prevedel."

echo "Izvajam osnovni test..."

# Preprost test, da se program vsaj zažene
./vaja5 c test.txt
./vaja5 d out.txt

echo "Vsi testi so uspešno zaključeni."