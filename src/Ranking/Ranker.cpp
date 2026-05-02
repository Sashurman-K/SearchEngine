#include "Ranker.h"

void Ranker::setStrategy(std::unique_ptr<IRankingStrategy> s) {
    strategy = std::move(s);
}

std::vector<Result> Ranker::rank(const QueryNode& query,
                                  const InvertedIndex& index) const
{
    if (strategy) {
        return strategy->rank(query, index);
    }
    return {};
}