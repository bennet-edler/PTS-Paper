# Implementation of ...

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

## Tests
```bash
./build/run_tests
```

### Build in Debug Mode
```bash
cmake -B build -D CMAKE_BUILD_TYPE=Debug
```


