#include <gtest/gtest.h>
#include "SearchEngine.h"
#include "InvertedIndex.h"
#include "Ranker.h"
#include "TFIDFStrategy.h"

// Тест 1: Поиск по пустому запросу не должен падать или вызывать стратегию
TEST(SearchEngineTest, EmptyQueryReturnsEmptyResult) {
    InvertedIndex idx;
    Ranker ranker;
    ranker.setStrategy(std::make_unique<TFIDFStrategy>());

    SearchEngine engine(std::move(idx), std::move(ranker));
    auto results = engine.search("");

    EXPECT_TRUE(results.empty());
}

// Тест 2: Поиск пробельного запроса
TEST(SearchEngineTest, SpacesOnlyQueryReturnsEmptyResult) {
    InvertedIndex idx;
    Ranker ranker;
    ranker.setStrategy(std::make_unique<TFIDFStrategy>());

    SearchEngine engine(std::move(idx), std::move(ranker));
    auto results = engine.search("     \t  ");

    EXPECT_TRUE(results.empty());
}