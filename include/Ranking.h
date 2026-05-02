#pragma once
#include "Types.h"
#include "InvertedIndex.h"
#include <vector>
#include <string>

class IRankingStrategy {
public:
    virtual ~IRankingStrategy() = default;
    virtual std::vector<Result> rank(const QueryNode& query, const InvertedIndex& index) = 0;
};

class TFIDFStrategy : public IRankingStrategy {
private:
    double calculateTFIDF(const std::string& term, int docId) const;
public:
    std::vector<Result> rank(const QueryNode& query, const InvertedIndex& index) override;
};

class BM25Strategy : public IRankingStrategy {
public:
    std::vector<Result> rank(const QueryNode& query, const InvertedIndex& index) override;
};