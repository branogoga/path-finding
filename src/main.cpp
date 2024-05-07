#include "version.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>

#include <iostream>

#include "graph.h"

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
}

int main() {
    std::cout << "Hello Path Finding " << getVersion() << "!" << std::endl;

    try {
        //
        //MapGraphLoader graphLoader("data\\test.map");
        DefaultGraphLoader graphLoader;
        auto graph = graphLoader.getGraph();
        print_graph(graph);

        Vertex start = vertex(0, graph);
        Vertex target = vertex(2, graph);
        auto path = shortest_path(graph, start, target);
        std::cout << "Shortest path from " << start << " to " << target << ": ";
        for(const auto& vertex : path) {
            std::cout << vertex << ", ";
        }
        std::cout << std::endl;

        std::cout << "Total length :" << path_length(graph, path) << std::endl;
    }
    catch (std::exception& exception) {
        std::cerr << "Uncaught exception: " << exception.what() << std::endl;
    }

    return 0;
}