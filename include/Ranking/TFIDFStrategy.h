#pragma once
#include "IRankingStrategy.h"

class TFIDFStrategy : public IRankingStrategy {
private:
    double calculateTFIDF(const std::string& term, int docId,
                          const InvertedIndex& index) const;
public:
    std::vector<Result> rank(const QueryNode& query,
                             const InvertedIndex& index) override;
};