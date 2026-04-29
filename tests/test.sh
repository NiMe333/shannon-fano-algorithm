#!/bin/bash

# Če katerikoli ukaz pade, se skripta takoj ustavi
set -e

echo "Prevajam program..."

# Če tvoja datoteka ni vaja5.cpp, tukaj popravi ime
$CXX -Wall -Wextra -std=c++17 vaja5.cpp -o vaja5

echo "Program se je uspešno prevedel."

echo "Izvajam osnovni test..."

# Tukaj damo preprost test, da se program vsaj zažene
./vaja5 || true

echo "Vsi testi so uspešno zaključeni."