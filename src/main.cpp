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

void print_graph(/*const*/ WeightedDiGraph& graph) {
    // represent graph in DOT format and send to cout
    boost::dynamic_properties property_writer;
    property_writer.property("node_id", get(boost::vertex_index, graph));
    property_writer.property("weight",  get(boost::edge_weight,  graph));
    write_graphviz_dp(std::cout, graph, property_writer);
    //write_graphviz
}

int main() {
    std::cout << "Hello Path Finding " << getVersion() << "!" << std::endl;

    try {
        //DefaultScenarioLoader scenarioLoader;
        const std::string scenarioFile = std::string(PROJECT_ROOT_DIR) + std::string("/data/maze-32-32-2/maze-32-32-2-even-1.scen");
        FileScenarioLoader scenarioLoader(std::filesystem::path(scenarioFile).make_preferred().string());
        const auto jobRequests = scenarioLoader.getjobRequests();
        auto graph = scenarioLoader.getGraph();
        print_graph(graph);

        for(const auto& jobRequest : jobRequests) {
            Vertex start = vertex(jobRequest.startVertex, graph);
            Vertex target = vertex(jobRequest.endVertex, graph);
            auto path = shortest_path(graph, start, target);
            std::cout << std::endl;
            std::cout << "Shortest path from " << start << " to " << target << ": " << std::endl;
            for(const auto& vertex : path) {
                std::cout << " - " << vertex << ": position = " << graph[vertex].position << std::endl;
            }
            std::cout << std::endl;

            std::cout << "Total length :" << path_length(graph, path) << std::endl;            
        }
    }
    catch (std::exception& exception) {
        std::cerr << "Uncaught exception: " << exception.what() << std::endl;
    }

    return 0;
}