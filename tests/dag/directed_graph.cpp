#include <gtest/gtest.h>

#include <lsqecc/dag/directed_graph.hpp>

using namespace lsqecc::dag;

TEST(directed_graph, add_node)
{
    DirectedGraph g;
    g.add_node(0);
    g.add_node(1);
    g.add_node(2);
    g.add_node(2);
    ASSERT_EQ(3, get_back_edges_for_testing(g).size());    
    ASSERT_EQ(3, get_edges_for_testing(g).size());    
    ASSERT_EQ(0, get_back_edges_for_testing(g).at(0).size());    
    ASSERT_EQ(0, get_edges_for_testing(g).at(0).size());    
    ASSERT_EQ(0, get_back_edges_for_testing(g).at(1).size());    
    ASSERT_EQ(0, get_edges_for_testing(g).at(1).size());    
    ASSERT_EQ(0, get_back_edges_for_testing(g).at(2).size());    
    ASSERT_EQ(0, get_edges_for_testing(g).at(2).size());    

}

TEST(directed_graph, add_edge)
{
    DirectedGraph g;
    g.add_edge(0, 1);
    ASSERT_EQ(2, get_back_edges_for_testing(g).size());
    ASSERT_EQ(2, get_edges_for_testing(g).size());
    ASSERT_EQ(Set<label_t>{1}, get_edges_for_testing(g).at(0));
    ASSERT_EQ(Set<label_t>{}, get_edges_for_testing(g).at(1));
    ASSERT_EQ(Set<label_t>{0}, get_back_edges_for_testing(g).at(1));
    ASSERT_EQ(Set<label_t>{}, get_back_edges_for_testing(g).at(0));

    g.add_edge(1, 2);
    ASSERT_EQ(3, get_back_edges_for_testing(g).size());
    ASSERT_EQ(3, get_edges_for_testing(g).size());
    ASSERT_EQ(Set<label_t>{1}, get_edges_for_testing(g).at(0));
    ASSERT_EQ(Set<label_t>{2}, get_edges_for_testing(g).at(1));
    ASSERT_EQ(Set<label_t>{}, get_edges_for_testing(g).at(2));
    ASSERT_EQ(Set<label_t>{0}, get_back_edges_for_testing(g).at(1));
    ASSERT_EQ(Set<label_t>{1}, get_back_edges_for_testing(g).at(2));
    ASSERT_EQ(Set<label_t>{}, get_back_edges_for_testing(g).at(0));


    g.add_edge(0, 2);
    ASSERT_EQ(3, get_back_edges_for_testing(g).size());
    ASSERT_EQ(3, get_edges_for_testing(g).size());
    Set<label_t> v{1, 2};
    ASSERT_EQ(v, get_edges_for_testing(g).at(0));
    ASSERT_EQ(Set<label_t>{2}, get_edges_for_testing(g).at(1));
    ASSERT_EQ(Set<label_t>{}, get_edges_for_testing(g).at(2));
    ASSERT_EQ(Set<label_t>{0}, get_back_edges_for_testing(g).at(1));
    Set<label_t> v2{1, 0};
    ASSERT_EQ(v2, get_back_edges_for_testing(g).at(2));
    ASSERT_EQ(Set<label_t>{}, get_back_edges_for_testing(g).at(0));
}


TEST(directed_graph, heads_and_tails)
{
    DirectedGraph g;
    g.add_edge(0, 2);
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    g.add_edge(2, 4);
    /* Graph (all edges pointing downwards):
    0   1
     \ /
      2
     / \
    3   4
    */
    Set<label_t> heads{0,1};
    ASSERT_EQ(heads, g.heads());
    Set<label_t> tails{3,4};
    ASSERT_EQ(tails, g.tails());
}

TEST(directed_graph, heads_and_tails_after_update)
{
    DirectedGraph g;
    g.add_edge(0, 2);
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    g.add_edge(2, 4);
    /* Graph (all edges pointing downwards):
    0   1
     \ /
      2
     / \
    3   4
    */

    g.add_edge(100, 0);
    g.remove_node(3);
    /* Graph after update (all edges pointing downwards):
    100
    |
    0   1
     \ /
      2
       \
        4
    */

    Set<label_t> heads{1, 100};
    ASSERT_EQ(heads, g.heads());
    Set<label_t> tails{4};
    ASSERT_EQ(tails, g.tails());
}


TEST(directed_graph, subdivide)
{
    DirectedGraph g;
    g.add_edge(0, 2);
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    g.add_edge(2, 4);
    /* Graph (all edges pointing downwards):
    0   1
     \ /
      2
     / \
    3   4
    */

    g.subdivide(2, {100, 101, 102});

    /* Graph after subdivide (all edges pointing downwards):
    0   1
     \ /
     100
      |
     101 
      |
     102
     / \
    3   4
    */

    ASSERT_EQ(7, get_back_edges_for_testing(g).size());
    ASSERT_EQ(7, get_edges_for_testing(g).size());
    ASSERT_EQ(Set<label_t>{100}, get_edges_for_testing(g).at(0));
    ASSERT_EQ(Set<label_t>{100}, get_edges_for_testing(g).at(1));
    ASSERT_EQ(Set<label_t>{101}, get_edges_for_testing(g).at(100));
    ASSERT_EQ(Set<label_t>{102}, get_edges_for_testing(g).at(101));
    Set<label_t> v{3, 4};
    ASSERT_EQ(v, get_edges_for_testing(g).at(102));
    ASSERT_EQ(Set<label_t>{}, get_edges_for_testing(g).at(3));
    ASSERT_EQ(Set<label_t>{}, get_edges_for_testing(g).at(4));

    ASSERT_EQ(Set<label_t>{}, get_back_edges_for_testing(g).at(0));
    ASSERT_EQ(Set<label_t>{}, get_back_edges_for_testing(g).at(1));
    Set<label_t> v2{0, 1};
    ASSERT_EQ(v2, get_back_edges_for_testing(g).at(100));
    ASSERT_EQ(Set<label_t>{100}, get_back_edges_for_testing(g).at(101));
    ASSERT_EQ(Set<label_t>{101}, get_back_edges_for_testing(g).at(102));
    ASSERT_EQ(Set<label_t>{102}, get_back_edges_for_testing(g).at(3));
    ASSERT_EQ(Set<label_t>{102}, get_back_edges_for_testing(g).at(4));

}