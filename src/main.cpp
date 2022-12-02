#include <iostream>
#include <sstream>

#include "version.h"

std::string getVersion() {
    std::ostringstream out;
    out << PathFinding_VERSION_MAJOR << "." << PathFinding_VERSION_MINOR << "." << PathFinding_VERSION_PATCH << "."
        << PathFinding_VERSION_TWEAK;
    return out.str();
}

int main() {
    std::cout << "Hello Path Finding" << getVersion() << "!" << std::endl;
    return 0;
}