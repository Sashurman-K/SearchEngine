#include "SearchEngine.h"

SearchEngine::SearchEngine(InvertedIndex idx, Ranker r)
    : index(std::move(idx)), ranker(std::move(r)) {}

std::vector<Result> SearchEngine::search(const std::string& query) {
    QueryNode parsedQuery = parser.parse(query);
    return ranker.rank(parsedQuery, index);
}