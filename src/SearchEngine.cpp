#include "SearchEngine.h"

SearchEngine::SearchEngine(InvertedIndex idx, Ranker r)
    : index(std::move(idx))
    , parser()
    , ranker(std::move(r))
{}

std::vector<Result> SearchEngine::search(const std::string& query) {
    // 1. Парсим запрос
    QueryNode queryNode = parser.parse(query);

    // 2. Если запрос пустой — возвращаем пустой результат
    if (queryNode.terms.empty()) {
        return {};
    }

    // 3. Ранжируем через выбранную стратегию
    return ranker.rank(queryNode, index);
}