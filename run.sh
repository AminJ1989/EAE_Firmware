#!/usr/bin/env bash
set -e

rm -rf build

cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build -j

cd build

ctest --output-on-failure

# Run main program with parameters
./canpid --kp 2.0 --ki 0.5 --kd 0.1 --setpoint 40 --ambient 25 --dt 0.1
