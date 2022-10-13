#pragma once

#include "common.h"
#include "graph.h"
#include "rewrite.h"

#include <sstream>
#include <string>

template<typename AlphabetKind, typename ArrowKind>
struct IGraph 
{
    virtual IndexType AddNode(AlphabetKind type) = 0;

    virtual void Link(IndexType source, IndexType target, ArrowKind arrow) = 0;
    virtual void Link(IndexType source, IndexType target) = 0;
    virtual void AddEdges(std::initializer_list<Binding> edges) = 0;

    virtual void Unlink(IndexType source, IndexType target) = 0;
    virtual void RemoveNode(IndexType index) = 0;

    virtual void ChangeKind(IndexType index, AlphabetKind type) = 0;
};

template<IndexType MaxSize,
    typename AlphabetKind, AlphabetKind Empty,
    typename ArrowKind, ArrowKind NotConnected>
class Graph : IGraph<AlphabetKind, ArrowKind>
{
public:
    using Rewriter = IRewrite<AlphabetKind, ArrowKind>;

    struct ActionAddNode : IRewrite<AlphabetKind, ArrowKind>
    {
        PlaceholderType index;
        AlphabetKind type;

        ActionAddNode(PlaceholderType index, AlphabetKind type)
            : index{index}
            , type{type}
        {}

        void Apply(BindingMap& binding, IGraph<AlphabetKind, ArrowKind>* graph) override
        {
            assert(binding.find(index) == binding.end());

            std::cout << "Adding node #" << (int)index << std::endl;
            binding[index] = graph->AddNode(type);
        }
    };

    struct ActionAddEdge : IRewrite<AlphabetKind, ArrowKind>
    {
        PlaceholderType source;
        PlaceholderType target;

        ActionAddEdge(PlaceholderType source, PlaceholderType target)
            : source{source}
            , target{target}
        {}

        void Apply(BindingMap& binding, IGraph<AlphabetKind, ArrowKind>* graph) override
        {
            assert(binding.find(source) != binding.end());
            assert(binding.find(target) != binding.end());
            
            std::cout << "Adding edge #" << (int)source << " -> #" << (int)target << std::endl;
            graph->Link(binding[source], binding[target]);
        }
    };

    struct ActionRemoveNode : IRewrite<AlphabetKind, ArrowKind>
    {
        PlaceholderType index;

        ActionRemoveNode(PlaceholderType index, AlphabetKind type)
            : index{ index }
        {}

        void Apply(BindingMap& binding, IGraph<AlphabetKind, ArrowKind>* graph) override
        {
            assert(binding.find(index) != binding.end());

            std::cout << "Removing node #" << (int)index << std::endl;
            graph->RemoveNode(binding[index]);
        }
    };

    struct ActionRemoveEdge : IRewrite<AlphabetKind, ArrowKind>
    {
        PlaceholderType source;
        PlaceholderType target;

        ActionRemoveEdge(PlaceholderType source, PlaceholderType target)
            : source{ source }
            , target{ target }
        {}

        void Apply(BindingMap& binding, IGraph<AlphabetKind, ArrowKind>* graph) override
        {
            assert(binding.find(source) != binding.end());
            assert(binding.find(target) != binding.end());

            std::cout << "Removing edge #" << (int)source << " -> #" << (int)target << std::endl;
            graph->Unlink(binding[source], binding[target]);
        }
    };

    struct ActionChangeKind : IRewrite<AlphabetKind, ArrowKind>
    {
        PlaceholderType index;
        AlphabetKind type;

        ActionChangeKind(PlaceholderType index, AlphabetKind type)
            : index{ index }
            , type{ type }
        {}

        void Apply(BindingMap& binding, IGraph<AlphabetKind, ArrowKind>* graph) override
        {
            assert(binding.find(index) != binding.end());

            std::cout << "Changing kind of node #" << (int)binding[index] << std::endl;
            graph->ChangeKind(binding[index], type);
        }
    };

private:
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

    template<typename Printer>
    std::string Dot(Printer alphabetToString) const
    {
        std::stringstream dot("");

        dot << "digraph G {" << std::endl;

        for (int i = 0; i < m_MaxUsed; i++)
        {
            if (m_Nodes[i] != Empty)
            {
                dot << "\t" << i << " [label = \"" << i << ":" << alphabetToString(m_Nodes[i]) << "\"]" << std::endl;
            }
        }

        for (int i = 0; i < m_MaxUsed; i++)
        {
            for (int j = 0; j < m_MaxUsed; j++)
            {
                if (m_Adjacent[i][j] != 0)
                {
                    dot << "\t" << i << " -> " << j << std::endl;
                }
            }
        }

        dot << "}" << std::endl;

        return dot.str();
    }

    std::string Dot() const
    {
        std::stringstream dot("");

        dot << "digraph G {" << std::endl;
        for (int i = 0; i < m_MaxUsed; i++)
        {
            for (int j = 0; j < m_MaxUsed; j++)
            {
                if (m_Adjacent[i][j] != 0)
                {
                    dot << "\t" << i << " -> " << j << std::endl;
                }
            }
        }

        dot << "}" << std::endl;

        return dot.str();
    }

    IndexType AddNode(AlphabetKind type)
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

    void Link(IndexType source, IndexType target)
    {
        Link(source, target, (ArrowKind)1);
    }

    void AddEdges(std::initializer_list<Binding> edges)
    {
        for (auto edge : edges)
        {
            Link(edge.first, edge.second);
        }
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

    void RemoveNode(IndexType index)
    {
        assert(index < m_MaxUsed);

        for (int i = 0; i < m_MaxUsed; i++)
        {
            for (int j = 0; j < m_MaxUsed; j++)
            {
                if (i == index || j == index)
                {
                    if (m_Adjacent[i][j] != 0)
                    {
                        Unlink(i, j);
                    }
                }
            }
        }
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

    void Rewrite(
        const Pattern<AlphabetKind>& find, 
        std::vector<IRewrite<AlphabetKind, ArrowKind>*>& repl, 
        const RewriteConfig& config)
    {
        auto match = Find(find);

        if (match.size() == 0) return;

        std::vector<BindingMap> matchesToUse{};

        switch (config.Target)
        {
        case ERewriteTarget::All:
            matchesToUse = match;
            break;

        case ERewriteTarget::One:
            matchesToUse.push_back(match[config.ChooseOneFn(match.size())]);
            break;
        }

        for (auto& match : matchesToUse)
        {
            std::cout << "Using match " << std::endl;
            for (auto entry : match)
            {
                std::cout << "\t#" << (int)entry.first << " = " << (int)entry.second << std::endl;
            }

            for (auto* action : repl)
            {
                action->Apply(match, this);
            }
        }
    }

    void ChangeKind(IndexType index, AlphabetKind type) override 
    {
        assert(index < m_MaxUsed);
        m_Nodes[index] = type;
    }
};

template<typename T>
using StandardGraph = Graph<32, T, (T)0, bool, false>;
