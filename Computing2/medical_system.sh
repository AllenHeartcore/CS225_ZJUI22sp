#!/bin/bash

clear
echo "Compiling..." 
g++ -w source_code/*.cpp -o medical_system
./medical_system
