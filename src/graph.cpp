#include "graph.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <filesystem>
#include <fstream>
#include <optional>

bool operator==(const Point2D& p1, const Point2D& p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

std::ostream& operator<<(std::ostream& stream, const Point2D& point) {
    stream << "[ " << point.x << "," << point.y << " ]";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const Path& path) {
    stream << "[";
    for(auto vertex : path) {
        stream << vertex << ", " << std::endl;
    }
    stream << "]";
    return stream;
}

Path extract_path(std::vector<Vertex> predecessor, const Vertex& target, const Vertex& start) {
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

Path shortest_path(const WeightedDiGraph& graph, const Vertex& start, const Vertex& target) {
    std::vector<Vertex> predecessor(num_vertices(graph));
    std::vector<int> distance(num_vertices(graph));
    boost::dijkstra_shortest_paths(graph, start, boost::predecessor_map(&predecessor[0]).distance_map(&distance[0]));
    auto path = extract_path(predecessor, target, start);
    return std::vector<Vertex>(path.rbegin(), path.rend());
}

unsigned path_length(const WeightedDiGraph& graph, const Path& path) {
    unsigned length = 0;
    for (size_t index = 0; index < path.size() - 1; ++index) {
        const std::pair<Edge, bool> edgeDescriptor = boost::edge(path[index], path[index + 1], graph);
        if (edgeDescriptor.second) {
            length += boost::get(boost::edge_weight_t(), graph, edgeDescriptor.first);
        }
    }
    return length;
}

/// @brief Returns shared vertices of two paths. Time complexity O(N*logN), where N is max. path length.
/// @param path1 
/// @param path2 
/// @return Sorted list of shared vertices
std::vector<Vertex> intersection(const Path& path1, const Path& path2) {
    Path vertices1 = path1;
    Path vertices2 = path2;
    std::sort(vertices1.begin(), vertices1.end());
    std::sort(vertices2.begin(), vertices2.end());
 
    std::vector<Vertex> intersection;
    std::set_intersection(vertices1.begin(), vertices1.end(), vertices2.begin(), vertices2.end(),
                          std::back_inserter(intersection));

    return intersection;
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

MapGraphLoader::MapGraphLoader(const std::string& filename): filename(filename), width(0), height(0) {
    graph = readFile();
}

WeightedDiGraph MapGraphLoader::getGraph() const {
    return graph;
}

const std::string& MapGraphLoader::getFilename() const {
    return filename;
}

std::optional<unsigned> MapGraphLoader::convertMapPositionToVertexIndex(size_t row, size_t column) const {
    return mapPositionToVertexIndex[row][column];
}

std::pair<size_t, size_t> MapGraphLoader::convertVertexIndexToMapPosition(unsigned vertex) const {
    return vertexIndexToMapPosition[vertex];
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
                graph.m_vertices[currentVertexIndex].m_property.position = {double(row), double(column)};
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
