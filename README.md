# Implementation of the Paper "Improved Approximation Algorithms for Parallel Task Scheduling and Multiple Cluster Scheduling".

# Requirements
- for program: c++ and cmake
- for tests:   google test
- for scripts: python3 with pandas and matplotlib

## Install Dependencies on nixos (optional)
```bash
nix-shell
```

# Building
```bash
cmake -B build -S .
```

# Compile
```bash
cmake --build build
```

## Execute
```bash
./build/program
```

## Plot Results
```bash
python3 scripts/plot_benchmark.py
```

## Tests
```bash
./build/run_tests
```
