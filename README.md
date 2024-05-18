# Path finding
Comparison of different algorithms for single &amp; multi agent path finding (_with application to real-world warehouse autonomous robot movement_)

**Assumptions:**
- arbitrary directed graph (_not restricted to grid, despite examples in data folder are mostly grids_)
- non-negative edge weights
- known position of the vertices
- constant robot speed (_always travels the same speed, no acceleration / deceleration_)
- constant robot speed on all edges (_e.g. full speed also on curved trajectories ..._)
- time is discrete
- robot appears at the start of the path and disappears when reached the destination

**Algorithms:**
- Dijkstra's algorithm

## How to run

### Prerequisities

#### Required
1. [CMake](https://cmake.org/)
2. [Connan package manager](https://conan.io/)

#### Recomended
3. [clang-format](https://releases.llvm.org/)

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
chmod +x path-finding
./path-finding
chmod +x path-finding-test
./path-finding-test
ctest
```

**Windows - MSVC**

To prevent [issues with spaces in the path](https://github.com/conan-io/conan/issues/16182#issuecomment-2088298270) (_e.g. when Windows user name has space in the path_) ensure, that path to Conan home directory does not contain spaces (_don't forget to restart command line in order environment to apply_):

```
CONAN_HOME="C:\.conan"
```

```
conan install . -s build_type=Debug --build missing
conan install . -s build_type=Release --build missing
cmake -DCMAKE_TOOLCHAIN_FILE="build/generators/conan_toolchain.cmake"
cmake --build .
ctest
```

## Visualize results
 Install [GraphViz](https://graphviz.org/download/). Make sure the path to installation is present in system path.

Check the `PROJECT_ROOT/output` directory for output.

 Convert resulting graph `.dot` file to visual `.png` image:

 `fdp -Tpng output/graph.dot -o output/graph.png`