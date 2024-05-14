#include "version.h"

#include "graph.h"
#include "scenario.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>

#include <iostream>
#include <filesystem>

std::string getVersion() {
    std::ostringstream out;
    out << PathFinding_VERSION_MAJOR << "." << PathFinding_VERSION_MINOR << "." << PathFinding_VERSION_PATCH << "."
        << PathFinding_VERSION_TWEAK;
    return out.str();
}

void print_graph_statistics(/*const*/ WeightedDiGraph& graph) {
    std::cout << "Graph:" << std::endl;
    std::cout << " - vertices: " << graph.m_vertices.size() << std::endl;
    //std::cout << " - edges: " << graph.m_edges.size() << std::endl;
    //std::cout << " - density: " << static_cast<float>(graph.m_edges.size()) / graph.m_vertices.size() << std::endl;
    std::cout << std::endl;
}

void print_graph_to_dot_file(/*const*/ WeightedDiGraph& graph) {
    // represent graph in DOT format and send to cout
    boost::dynamic_properties property_writer;
    property_writer.property("node_id", get(boost::vertex_index, graph));
    property_writer.property("weight",  get(boost::edge_weight,  graph));
    write_graphviz_dp(std::cout, graph, property_writer);
    //write_graphviz
}

std::vector<Path> calculate_shortest_paths(const std::vector<JobRequest>& jobRequests, const WeightedDiGraph& graph, const unsigned numberOfRobots) {
    std::vector<Path> paths;
    for(unsigned robotIndex = 0; robotIndex <  numberOfRobots; ++robotIndex) {
        const auto& jobRequest = jobRequests[robotIndex];
        Vertex start = vertex(jobRequest.startVertex, graph);
        Vertex target = vertex(jobRequest.endVertex, graph);
        paths.emplace_back(shortest_path(graph, start, target));
    }
    return paths;
}

void print_paths(const std::vector<Path>& paths, const WeightedDiGraph& graph) {
    for(const auto& path : paths) {
        std::cout << std::endl;
        const auto start = path[0];
        const auto target = path[path.size()-1];
        std::cout << "Shortest path from " << start << " to " << target << " (" << path.size() << " vertices, length " << path_length(graph, path) << ")" << std::endl;
        // for(const auto& vertex : path) {
        //     std::cout << " - " << vertex << ": position = " << graph[vertex].position << std::endl;
        // }
        // std::cout << std::endl;

        // std::cout << "Number of vertices:" << path.size() << std::endl;            
        // std::cout << "Total length :" << path_length(graph, path) << std::endl;            
    }
}

const std::string Maze_32x32_2_Even_1 = "/data/maze-32-32-2/maze-32-32-2-even-1.scen";

int main() {
    std::cout << "Hello Path Finding " << getVersion() << "!" << std::endl;

    try {
        //DefaultScenarioLoader scenarioLoader;
        const std::string scenarioFile = std::string(PROJECT_ROOT_DIR) + Maze_32x32_2_Even_1;
        FileScenarioLoader scenarioLoader(std::filesystem::path(scenarioFile).make_preferred().string());
        const auto jobRequests = scenarioLoader.getjobRequests();
        auto graph = scenarioLoader.getGraph();
        print_graph_statistics(graph);
        //print_graph_to_dot_file(graph);

        const unsigned numberOfRobots = 5;
        std::vector<Path> paths = calculate_shortest_paths(jobRequests, graph, numberOfRobots);
        print_paths(paths, graph);

        typedef std::tuple<unsigned, unsigned, std::vector<Vertex>> Intersection;
        std::vector<Intersection> intersections;
        for(unsigned i = 0; i < paths.size(); ++i) {
            for(unsigned j=0; j < paths.size(); ++j) {
                if(i < j) {
                    const auto shared_vertices = intersection(paths[i], paths[j]);
                    if(!shared_vertices.empty()) {
                        intersections.emplace_back(i, j, shared_vertices);
                    }
                }
            }
        }

        std::cout << "Intersections (" << intersections.size() << "):" << std::endl;
        for(const auto& intersection : intersections) {
            const auto [i, j, shared_vertices] = intersection;
            std::cout << " - " << i << ", " << j << ", " << "[";
            for(const auto& vertex : shared_vertices) {
                std::cout << vertex << ", ";
            }
            std::cout << "]" << std::endl;
        }
    }
    catch (std::exception& exception) {
        std::cerr << "Uncaught exception: " << exception.what() << std::endl;
    }

    return 0;
}