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

void print_graph() {
    typedef boost::adjacency_list< boost::listS, boost::vecS, boost::directedS > digraph;

    // instantiate a digraph object with 8 vertices
    digraph g(8);

    // add some edges
    add_edge(0, 1, g);
    add_edge(1, 5, g);
    add_edge(5, 6, g);
    add_edge(2, 3, g);
    add_edge(2, 4, g);
    add_edge(3, 5, g);
    add_edge(4, 5, g);
    add_edge(5, 7, g);

    // represent graph in DOT format and send to cout
    write_graphviz(std::cout, g);
}

int main() {
    std::cout << "Hello Path Finding" << getVersion() << "!" << std::endl;
    print_graph();
    return 0;
}