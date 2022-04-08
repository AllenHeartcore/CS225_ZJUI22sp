#!/bin/bash

clear
echo "Compiling..." 
g++ -w -o medical_system source_code/main.cpp source_code/fibonacci_heap.cpp source_code/controlflow.cpp source_code/load_data.cpp source_code/local_registration.cpp
./medical_system