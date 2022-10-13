#pragma once

#include <functional>

#include "common.h"

enum ERewriteTarget
{
	One,
	All,
};

struct RewriteConfig
{
	ERewriteTarget Target = ERewriteTarget::All;
	std::function<size_t(size_t)> ChooseOneFn = [](size_t i) { return 0; };
};

template<typename A, typename B>
struct IGraph;

template<typename AlphabetKind, typename ArrowKind>
struct IRewrite
{
	virtual ~IRewrite() = default;	
	virtual void Apply(BindingMap& binding, IGraph<AlphabetKind, ArrowKind>* graph) = 0;
};
