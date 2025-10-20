# EAE_Firmware

This project simulates a **cooling control system** using a **PID controller** and a **virtual CAN bus**.  
It models exchanging datahow between a controller and an ECU (temperature, ignition status, pump and fan commands)
over a CANBus network to regulate system temperature.

---

## Overview

| Module | Description |
|---------|-------------|
| `can_bus.c` / `.h` | In-memory CAN bus simulation using ring buffers |
| `pid.c` / `.h` | PID controller implementation |
| `fsm.c` / `.h` | Finite-state machine for cooling logic |
| `plant.c` / `.h` | Thermal plant model (ambient, fan, and pump effects) |
| `cli.c` / `.h` | Command-line argument parsing and defaults |
| `main.c` | Integrates all modules (CAN, PID, FSM, Plant, CLI) |

Unit tests (GoogleTest) verify PID and CAN bus behavior.

---

## Requirements
- **CMake ≥ 3.20**
- **C compiler** (GCC, Clang, or AppleClang)
- **Optional:** GoogleTest (fetched automatically)

Tested on:
- Ubuntu (gcc/clang)
- macOS (AppleClang)

---

## Build & Run

### Clone the repository
```bash
git clone https://github.com/<your-username>/<your-repo>.git
cd <your-repo>
```

### Make the build script executable *(first time only)*
```bash
chmod +x run.sh
```

### Build, test, and run
```bash
./run.sh
```
This script:
- Configures and builds the project with CMake  
- Runs all unit tests via CTest  
- Launches the simulation using default parameters  

---

## Manual Build (optional)
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
cmake --build build -j
ctest --test-dir build --output-on-failure
./build/canpid --ambient 25 --setpoint 40 --duration 10
```

---

## CLI Parameters

You can provide only the parameters you want; missing values use defaults.

| Flag | Description | Default |
|------|--------------|----------|
| `--ambient` | Ambient temperature (°C) | 25.0 |
| `--setpoint` | Target temperature (°C) | 40.0 |
| `--kp` | Proportional gain | 2.0 |
| `--ki` | Integral gain | 0.5 |
| `--kd` | Derivative gain | 0.2 |
| `--duration` | Simulation duration (s) | 20.0 |
| `--ignition` | Ignition ON/OFF (1 or 0) | 1 |
| `--fan_delta` | Fan-on temperature delta (°C) | 2.0 |
| `--help` | Show help message | — |

---

## Example
```bash
./build/canpid   --kp 2.0 --ki 0.5 --kd 0.1   --setpoint 40   --ambient 25   --duration 10   --ignition 1   --fan_delta 2.0
```

### Output
```
[Params] ambient=25.0C setpoint=40.0C kp=2.00 ki=0.50 kd=0.10 duration=10.0s ignition=1 fan_delta=2.0C
t= 0.1s  ST=1  Temp=45.00C  Set=40.00C  Pump= 20.0%  Fan=  0.0%
t= 0.2s  ST=1  Temp=43.20C  Set=40.00C  Pump= 15.5%  Fan=  0.0%
...
t=10.0s  ST=2  Temp=40.10C  Set=40.00C  Pump=  5.0%  Fan=  0.0%
Done.
```

---

## Project Structure
```
.
├── src/
│   ├── main.c
│   ├── can_bus.c
│   ├── pid.c
│   ├── fsm.c
│   ├── plant.c
│   └── cli.c
├── include/
│   ├── can_bus.h
│   ├── pid.h
│   ├── fsm.h
│   ├── plant.h
│   └── cli.h
├── tests/
│   ├── test_pid.cpp
│   └── test_can.cpp
├── run.sh
├── CMakeLists.txt
└── README.md
```

---

## Continuous Integration (GitHub Actions)

The project uses GitHub Actions for automated CI testing.
Builds the project on Ubuntu
Runs all unit tests (GoogleTest and CTest)
Executes the main CLI simulation to verify correct behavior
Workflow file:
.github/workflows/ci.yml

---

## Notes
- Written in **C**, with tests in **C++ (GoogleTest)**  
- Works identically on macOS and Linux  
- Time step `dt = 0.5 s` is fixed in `main.c`

---

## Author
**Amin Jamalkhah**  

