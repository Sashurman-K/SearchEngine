#pragma once
#include "Types.h"
#include "InvertedIndex.h"
#include <vector>

class IRankingStrategy {
public:
    virtual ~IRankingStrategy() = default;
    virtual std::vector<Result> rank(const QueryNode& query,
                                      const InvertedIndex& index) = 0;
};