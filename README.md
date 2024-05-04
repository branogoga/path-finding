# Path finding
Comparison of different algorithms for single &amp; multi agent path finding (_with application to real-world warehouse autonomous robot movement_)

## How to run

### Prerequisities
1. [CMake](https://cmake.org/)
2. [Connan package manager](https://conan.io/)

### Build and run

**Docker**
```
docker build . -t path-finding:dev
docker run -it path-finding:dev
cd source
./path-finding
./path-finding-test
```

**Linux:**
```
mkdir .conan && cd .conan
conan install .. -s compiler.libcxx=libstdc++11
cd ..
cmake .
make
chmod +x path-finder
./path-finder
```

**Windows - MSVC**
```
mkdir .conan && cd .conan

```
