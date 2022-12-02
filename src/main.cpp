#include <iostream>
#include <sstream>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>

#include "version.h"

std::string getVersion() {
    std::ostringstream out;
    out << PathFinding_VERSION_MAJOR << "." << PathFinding_VERSION_MINOR << "." << PathFinding_VERSION_PATCH << "."
        << PathFinding_VERSION_TWEAK;
    return out.str();
}

typedef boost::adjacency_list< boost::listS, boost::vecS, boost::directedS > digraph;

digraph create_graph() {
    // instantiate a digraph object with 8 vertices
    digraph graph(8);

    // add some edges
    add_edge(0, 1, graph);
    add_edge(1, 5, graph);
    add_edge(5, 6, graph);
    add_edge(2, 3, graph);
    add_edge(2, 4, graph);
    add_edge(3, 5, graph);
    add_edge(4, 5, graph);
    add_edge(5, 7, graph);

    return graph;
}

void print_graph(const digraph& graph) {
    // represent graph in DOT format and send to cout
    write_graphviz(std::cout, graph);
}

int main() {
    std::cout << "Hello Path Finding" << getVersion() << "!" << std::endl;
    print_graph(create_graph());
    return 0;
}