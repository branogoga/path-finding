#include <gtest/gtest.h>

#include "graph.h"

TEST(DefaultGraphLoader, creates_graph_with_4_vertices) {
    DefaultGraphLoader loader;
    const auto graph = loader.getGraph();
    ASSERT_EQ(graph.m_vertices.size(), 4);
}


TEST(MapGraphLoader, loads_test_data) {
    MapGraphLoader loader("..\\data\\test.map");
    const auto graph = loader.getGraph();
    ASSERT_EQ(graph.m_vertices.size(), 3);
}

