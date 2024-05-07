#include "graph.h"

#include <gtest/gtest.h>
#include <filesystem>

TEST(DefaultGraphLoader, creates_graph_with_4_vertices) {
    DefaultGraphLoader loader;
    const auto graph = loader.getGraph();
    ASSERT_EQ(graph.m_vertices.size(), 4);
}


TEST(MapGraphLoader, loads_test_data) {
    std::cerr << "TEST OUTPUT" << std::endl;
    MapGraphLoader loader(std::filesystem::path(std::string(PROJECT_ROOT_DIR) + "/data/test.map").make_preferred().string());
    const auto graph = loader.getGraph();
    ASSERT_EQ(graph.m_vertices.size(), 3);
}

