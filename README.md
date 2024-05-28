# Path finding
Comparison of different algorithms for single &amp; multi agent path finding (_with application to real-world warehouse autonomous robot movement_)

**Assumptions:**
- arbitrary directed graph (_not restricted to grid, despite examples in data folder are mostly grids_)
- non-negative edge weights
- known position of the vertices
- agent is infinitely small (when calculating collisions)
- constant robot speed (_always travels the same speed, no acceleration / deceleration_)
- constant robot speed on all edges (_e.g. full speed also on curved trajectories ..._)
- time is discrete
- robot appears at the start of the path and disappears when reached the destination
- robots are reliable, they execute orders exactly as planned
- there is no delay in communicstion with robots

**Algorithms:**
- Dijkstra's algorithm
- A" search

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

 or run

 `./make_graph.bat` (windows)
 `./make_graph.sh` (linux)

 to convert all `dot` files in repo to corresponding `png` images.

 **Warning:** Converting all images can be slow depending on the number and size of the rendered graphs.

## File formats

Taken from [movingai.com](https://movingai.com/benchmarks/formats.html). You can download the test data there as well.

### Map format (`.map`)

The maps have the following format:
All maps begin with the lines:

```
type octile
height y
width x
map
```

where `y` and `x` are the respective height and width of the map.
The map data is store as an ASCII grid. The upper-left corner of the map is `(0,0)`. The following characters are possible:

```
. - passable terrain
G - passable terrain
@ - out of bounds
O - out of bounds
T - trees (unpassable)
S - swamp (passable from regular terrain)
W - water (traversable, but not passable from terrain)
```

### Scenario format (`.scen`)

Code to load the scenario files is available as part of HOG2, or can be accessed here: [ScenarioLoader.cpp](https://github.com/nathansttt/hog2/blob/PDB-refactor/utils/ScenarioLoader.cpp) and [ScenarioLoader.h](https://github.com/nathansttt/hog2/blob/PDB-refactor/utils/ScenarioLoader.h)
The scenario files have the following format.

The begin with the text `version x.x`. This document describes version `1.0`. The trailing `0` is optional.
Each line of a scenario has 9 fields:
```
Bucket	map	map width	map height	start x-coordinate	start y-coordinate	goal x-coordinate	goal y-coordinate	optimal length
0	maps/dao/arena.map	49	49	1	11	1	12	1
0	maps/dao/arena.map	49	49	1	13	4	12	3.41421
```

**Notes:**
The optimal path length is assuming `sqrt(2)` diagonal costs.
The optimal path length assumes agents cannot cut corners through walls
If the map height/width do not match the file, it should be scaled to that size
`(0, 0)` is in the upper left corner of the maps
Technically a single scenario file can have problems from many different maps, but currently every scenario only contains problems from a single map