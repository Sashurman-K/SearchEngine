#pragma once
#include "IRankingStrategy.h"
#include "Types.h"
#include <memory>
#include <vector>

class Ranker {
private:
    std::unique_ptr<IRankingStrategy> strategy;

public:
    void setStrategy(std::unique_ptr<IRankingStrategy> s);
    std::vector<Result> rank(const QueryNode& query,
                             const InvertedIndex& index) const;
};