#include "graph.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <filesystem>
#include <fstream>
#include <optional>

std::vector<Vertex> extract_path(std::vector<Vertex> predecessor, const Vertex& target, const Vertex& start) {
    std::vector<boost::graph_traits<WeightedDiGraph>::vertex_descriptor > path;
    boost::graph_traits<WeightedDiGraph>::vertex_descriptor current = target;

    while(current!=start)
    {
        path.push_back(current);
        current = predecessor[current];
    }
    path.push_back(start);
    return path;
}

std::vector<Vertex> shortest_path(const WeightedDiGraph& graph, const Vertex& start, const Vertex& target) {
    std::vector<Vertex> predecessor(num_vertices(graph));
    std::vector<int> distance(num_vertices(graph));
    boost::dijkstra_shortest_paths(graph, start, boost::predecessor_map(&predecessor[0]).distance_map(&distance[0]));
    auto path = extract_path(predecessor, target, start);
    return std::vector<Vertex>(path.rbegin(), path.rend());
}

WeightedDiGraph DefaultGraphLoader::getGraph() const {
    WeightedDiGraph graph(4);
    add_edge(0, 1, 2.0f, graph);
    add_edge(0, 2, 3.0f, graph);
    add_edge(1, 2, 1.0f, graph);
    add_edge(0, 3, 1.0f, graph);
    add_edge(3, 2, 1.0f, graph);

    return graph;
}

bool isVertexPassable(char value) {
    return value == '.' || value == 'G';
}

void readStaticHeaderLine(std::ifstream& file, const std::string expectedLine, const std::string& filename) {
    std::string line;
    std::getline(file, line);
    if (line != expectedLine) {
        std::ostringstream message;
        message << "Failed to open '" << filename << "': invalid format. Expected 'type octile', got '" << line << "'";
        throw std::invalid_argument(message.str());
    }
}

unsigned readUnsignedValue(std::ifstream& file, const std::string expectedParameterTitle, const std::string& filename) {
    std::string line;
    std::getline(file, line);
    std::string label;
    std::string stringValue;
    std::istringstream lineStream(line);
    std::getline(lineStream, label, ' ');
    if (label != expectedParameterTitle) {
        std::ostringstream message;
        message << "Failed to open '" << filename << "': invalid format. Expected 'type octile', got '" << label << "'";
        throw std::invalid_argument(message.str());
    }
    std::getline(lineStream, stringValue, ' ');
    int value = std::stoi(stringValue);
    if (value < 0) {
        std::ostringstream message;
        message << "Failed to open '" << filename << "': invalid format. Expected height to be positive, got '" << value << "'";
        throw std::invalid_argument(message.str());
    }
    return static_cast<unsigned>(value);
}

std::vector<std::string> readMap(std::ifstream& file, unsigned width, unsigned height, const std::string& filename) {
    std::vector<std::string> map;
    map.reserve(height);
    for (int h = 0; h < height; ++h) {
        std::string line;
        std::getline(file, line);
        map.push_back(line);
        if (line.size() != width) {
            std::ostringstream message;
            message << "Failed to open '" << filename << "': invalid format. Expected map row " << h << " will have " << width << " characters, but got " << line.size();
            throw std::invalid_argument(message.str());
        }
    }
    return map;
}

MapGraphLoader::MapGraphLoader(const std::string& filename): filename(filename) {
    graph = readFile();
}

WeightedDiGraph MapGraphLoader::getGraph() const {
    return graph;
}

// Map Format (https://movingai.com/benchmarks/formats.html)
// 
// The maps have the following format :
// All maps begin with the lines :
//
// type octile
// height y
// width x
// map
// where y and x are the respective height and width of the map.
// The map data is store as an ASCII grid.The upper - left corner of the map is(0, 0).The following characters are possible :
//
// . - passable terrain
// G - passable terrain
// @ - out of bounds
// O - out of bounds
// T - trees(unpassable)
// S - swamp(passable from regular terrain)
// W - water(traversable, but not passable from terrain)
WeightedDiGraph MapGraphLoader::readFile() {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::ostringstream message;
        message << "Failed to open '" << filename << "' from " << std::filesystem::current_path();
        throw std::runtime_error(message.str());
    }

    readStaticHeaderLine(file, "type octile", filename);
    height = readUnsignedValue(file, "height", filename);
    width = readUnsignedValue(file, "width", filename);
    readStaticHeaderLine(file, "map", filename);

    map = readMap(file, width, height, filename);

    mapPositionToVertexIndex = std::vector<std::vector<std::optional<unsigned>>>(height, std::vector<std::optional<unsigned>>(width, std::nullopt));
    vertexIndexToMapPosition = std::vector<std::pair<size_t, size_t>>();
    unsigned numberOfPassableNodes = 0;
    for (size_t row = 0; row < height; ++row) {
        for (size_t column = 0; column < width; ++column) {
            const char& value = map[row][column];
            if (isVertexPassable(value)) {
                vertexIndexToMapPosition.emplace_back(std::make_pair(row, column));
                mapPositionToVertexIndex[row][column] = numberOfPassableNodes;
                ++numberOfPassableNodes;
            }
        }
    }

    WeightedDiGraph graph(numberOfPassableNodes);
    for (size_t row = 0; row < height; ++row) {
        for (size_t column = 0; column < width; ++column) {
            const char& value = map[row][column];
            if (isVertexPassable(value)) {
                size_t currentVertexIndex = *mapPositionToVertexIndex[row][column];
                // Add edge to the left
                if (column > 0 && isVertexPassable(map[row][column-1])) {
                    std::optional<unsigned> leftVertexIndex = mapPositionToVertexIndex[row][column-1];
                    if(leftVertexIndex) {
                        add_edge(
                            currentVertexIndex,
                            *leftVertexIndex,
                            1.0f, 
                            graph
                        );
                        add_edge(
                            *leftVertexIndex,
                            currentVertexIndex,
                            1.0f,
                            graph
                        );
                    }
                }
                // Add edge to the top
                if (row > 0 && isVertexPassable(map[row-1][column])) {
                    std::optional<unsigned> topVertexIndex = mapPositionToVertexIndex[row-1][column];
                    if (topVertexIndex) {
                        add_edge(
                            currentVertexIndex,
                            *topVertexIndex,
                            1.0f,
                            graph
                        );
                        add_edge(
                            *topVertexIndex,
                            currentVertexIndex,
                            1.0f,
                            graph
                        );
                    }
                }
            }
        }
    }
    return graph;
}
