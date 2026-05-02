#pragma once
#include "InvertedIndex.h"
#include "QueryParser.h"
#include "Ranker.h"
#include <string>
#include <vector>

class SearchEngine {
private:
    InvertedIndex index;
    QueryParser parser;
    Ranker ranker;
public:
    SearchEngine(InvertedIndex idx, Ranker r);
    std::vector<Result> search(const std::string& query);
};