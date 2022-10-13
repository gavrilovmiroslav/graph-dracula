#pragma once

#include <cstdint>
#include <cassert>
#include <unordered_set>
#include <unordered_map>
#include <vector>

typedef uint8_t PlaceholderType;
typedef uint8_t IndexType;
typedef std::pair<PlaceholderType, IndexType> Binding;
typedef std::unordered_map<PlaceholderType, IndexType> BindingMap;