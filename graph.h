#pragma once

#include "common.h"
#include "graph.h"

template<IndexType MaxSize,
    typename AlphabetKind, AlphabetKind Empty,
    typename ArrowKind, ArrowKind NotConnected>
class Graph
{
    IndexType m_MaxUsed;

    AlphabetKind m_Nodes[MaxSize];
    ArrowKind m_Adjacent[MaxSize][MaxSize];

    uint8_t m_InCount[MaxSize];
    uint8_t m_OutCount[MaxSize];

public:
    Graph()
        : m_MaxUsed{ 0 }
        , m_Nodes{ Empty, }
        , m_Adjacent{ NotConnected, }
        , m_InCount{ 0, }
        , m_OutCount{ 0, }
    {}

    IndexType Add(AlphabetKind type)
    {
        static IndexType s_CurrentIndex = 0;
        m_Nodes[s_CurrentIndex] = type;
        m_MaxUsed++;
        return s_CurrentIndex++;
    }

    void Link(IndexType source, IndexType target, ArrowKind arrow)
    {
        assert(source < MaxSize);
        assert(source < m_MaxUsed);
        assert(target < MaxSize);
        assert(target < m_MaxUsed);
        assert(m_Nodes[source] != Empty);
        assert(m_Nodes[target] != Empty);
        assert(m_Adjacent[source][target] == NotConnected);

        m_Adjacent[source][target] = arrow;
        m_InCount[target]++;
        m_OutCount[source]++;
    }

    void Unlink(IndexType source, IndexType target)
    {
        assert(source < MaxSize);
        assert(source < m_MaxUsed);
        assert(target < MaxSize);
        assert(target < m_MaxUsed);
        assert(m_Nodes[source] != Empty);
        assert(m_Nodes[target] != Empty);
        assert(m_Adjacent[source][target] != NotConnected);

        m_Adjacent[source][target] = NotConnected;
        m_InCount[target]--;
        m_OutCount[source]--;
    }

    void Backtrack(const Pattern<AlphabetKind>& pattern, BindingMap map,
        std::unordered_map<PlaceholderType, std::unordered_set<IndexType>>& candidates,
        PlaceholderType current, std::vector<BindingMap>& results)
    {
        if (candidates.find(current) != candidates.end())
        {
            for (auto candidate : candidates[current])
            {
                map[current] = candidate;
                Backtrack(pattern, map, candidates, current + 1, results);
            }
        }
        else
        {
            std::unordered_set<IndexType> unique_values{};
            for (auto mapping : map)
            {
                unique_values.insert(mapping.second);
            }

            if (unique_values.size() == map.size())
            {
                for (auto arrow : pattern.Arrows())
                {
                    auto source = map[arrow.first];
                    auto target = map[arrow.second];
                    if (!m_Adjacent[source][target])
                    {
                        return;
                    }
                }

                results.push_back(map);
            }
        }
    }

    std::vector<BindingMap> Find(const Pattern<AlphabetKind>& pattern)
    {
        std::unordered_map<PlaceholderType, std::unordered_set<IndexType>> candidates;

        size_t patternIn[MaxSize]{ 0, };
        size_t patternOut[MaxSize]{ 0, };

        for (const auto& arrow : pattern.Arrows())
        {
            patternOut[arrow.first]++;
            patternIn[arrow.second]++;
        }

        for (const auto& node : pattern.Nodes())
        {
            auto minIn = patternIn[node.index];
            auto minOut = patternOut[node.index];

            candidates[node.index] = std::unordered_set<IndexType>{};
            for (int i = 0; i < m_MaxUsed; i++)
            {
                if (m_InCount[i] >= minIn && m_OutCount[i] >= minOut)
                {
                    if (node.value == Empty || m_Nodes[i] == node.value)
                    {
                        candidates[node.index].insert(i);
                    }
                }
            }
        }

        BindingMap map{};
        std::vector<BindingMap> results;
        Backtrack(pattern, map, candidates, 0, results);
        return results;
    }
};

template<typename T>
using StandardGraph = Graph<32, T, (T)0, bool, false>;
