#pragma once
#include "IRankingStrategy.h"

class BM25Strategy : public IRankingStrategy {
private:
    double k1 = 1.2;   // насыщение term frequency
    double b = 0.75;   // регуляризация длины документа

public:
    void setParameters(double k1, double b);

    std::vector<Result> rank(const QueryNode& query,
                             const InvertedIndex& index) override;
};