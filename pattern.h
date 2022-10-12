#pragma once

#include <iostream>
#include <cstdint>
#include <cassert>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "common.h"

template<typename AlphabetKind>
struct PatternNode {
    IndexType index;
    AlphabetKind value;

    bool operator==(const PatternNode<AlphabetKind>& other) const
    {
        return index == other.index;
    }

    struct HashFunction
    {
        size_t operator()(const PatternNode<AlphabetKind>& node) const
        {
            return std::hash<int>()(node.index);
        }
    };
};

template<typename AlphabetKind>
class Pattern {
    using Node = PatternNode<AlphabetKind>;
    using NodeSet = std::unordered_set<Node, PatternNode<AlphabetKind>::template HashFunction>;
    NodeSet m_Nodes;
    std::vector<Binding> m_Connections;

public:
    Pattern(std::initializer_list<Node> nodes, std::initializer_list<Binding> arrows)
        : m_Nodes{ nodes }
        , m_Connections{ arrows }
    {}

    const std::vector<Binding>& Arrows() const {
        return m_Connections;
    }

    const NodeSet& Nodes() const {
        return m_Nodes;
    }

    size_t NodeCount() const
    {
        return m_Nodes.size();
    }
};

#define to ,