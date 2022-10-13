#include <iostream>
#include <ctime>

#include "pattern.h"
#include "rewrite.h"
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

std::string StringifyNode(Node node)
{
    switch (node)
    {
    case Node::Any: return "?";
    case Node::Start: return "S";
    case Node::Entrance: return "e";
    case Node::Task: return "t";
    case Node::TaskProgress: return "T";
    case Node::Goal: return "g";
    case Node::Lock: return "l";
    case Node::Key: return "k";
    }
}

int main()
{
    using G = StandardGraph<Node>;
    G graph;
    auto a = graph.AddNode(Node::Start);
    auto b = graph.AddNode(Node::Entrance);
    auto c = graph.AddNode(Node::Goal);
    auto d = graph.AddNode(Node::TaskProgress);
    auto e = graph.AddNode(Node::TaskProgress);

    graph.AddEdges({
        { a , b }, { a , d }, { a , e },
        { b , c }, { d , c }, { e , d }
    });

    /*
        Start a0 ------> Entrance b1 -----> Goal c2
         | |                                  ^
         | |                                  |
         | \----> TaskProgress d3 ------------/
         |                ^
         v                |
         TaskProgress e4 --/
    */

    Pattern<Node> find{
        { 
            { 0, Node::Any }, 
            { 1, Node::TaskProgress }, 
            { 2, Node::Any } 
        },
        { { 0 , 1 }, { 1 , 2 } }
    };

    std::vector<G::Rewriter*> rewrite{
        new G::ActionAddNode(3, Node::Key),
        new G::ActionChangeKind(1, Node::Task),
        new G::ActionAddEdge(1, 3),
    };

    auto match = graph.Find(find);

    std::cout << graph.Dot(StringifyNode) << std::endl;

    RewriteConfig config;
    config.Target = ERewriteTarget::One;
    config.ChooseOneFn = [](size_t in) -> size_t { return (size_t)(rand() % in); };

    srand(time(0));

    graph.Rewrite(find, rewrite, config);
    graph.Rewrite(find, rewrite, config);
    graph.Rewrite(find, rewrite, config);
    graph.Rewrite(find, rewrite, config);

    std::cout << graph.Dot(StringifyNode) << std::endl;

    return 0;
}