#include <iostream>
#include <cstdint>
#include <cassert>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "pattern.h"
#include "graph.h"

enum class Node
{
    Any = 0,
    Start,
    Entrance,
    Task,
    TaskProgress,
    Goal,
    Lock,
    Key,
};

int main()
{
    StandardGraph<Node> graph;
    auto a = graph.Add(Node::Start);
    auto b = graph.Add(Node::Entrance);
    auto c = graph.Add(Node::Goal);
    auto d = graph.Add(Node::TaskProgress);
    auto e = graph.Add(Node::TaskProgress);
    graph.Link(a, b, true);
    graph.Link(b, c, true);
    graph.Link(a, d, true);
    graph.Link(a, e, true);
    graph.Link(d, c, true);
    graph.Link(e, d, true);

    /*
        Start a0 ------> Entrance b1 -----> Goal c2
         | |                                  ^
         | |                                  |
         | \----> TaskProgress d3 ------------/
         |                ^
         v                |
         TaskProgress e4 --/
    */

    Pattern<Node> pat{
        { { 0, Node::Any }, { 1, Node::TaskProgress }, { 2, Node::Any } },
        { { 0 to 1 }, { 1 to 2 } }
    };

    auto match = graph.Find(pat);
    for (auto mapping : match)
    {
        for (auto entry : mapping)
        {
            std::cout << "#" << (int)entry.first << " = " << (int)entry.second << std::endl;
        }
        std::cout << "--------------------------------" << std::endl;
    }
    return 0;
}