#include <gtest/gtest.h>
#include "InvertedIndex.h"
#include "Ranker.h"
#include "TFIDFStrategy.h"
#include "BM25Strategy.h"
#include "QueryParser.h" // Для QueryNode

class RankingTest : public ::testing::Test {
protected:
    InvertedIndex index;
    QueryNode query;

    void SetUp() override {
        // Документ 1: "hello world" (длина 2)
        Document doc1; doc1.id = 1; doc1.title = "doc1"; doc1.path = ""; doc1.size = 0;
        index.addDocument(1, doc1);
        index.setDocLength(1, 2);
        index.addTerm("hello", 1, 0);
        index.addTerm("world", 1, 1);

        // Документ 2: "hello cplusplus" (длина 2)
        Document doc2; doc2.id = 2; doc2.title = "doc2"; doc2.path = ""; doc2.size = 0;
        index.addDocument(2, doc2);
        index.setDocLength(2, 2);
        index.addTerm("hello", 2, 0);
        index.addTerm("cplusplus", 2, 1);

        // Документ 3: "world world world" (длина 3)
        Document doc3; doc3.id = 3; doc3.title = "doc3"; doc3.path = ""; doc3.size = 0;
        index.addDocument(3, doc3);
        index.setDocLength(3, 3);
        index.addTerm("world", 3, 0);
        index.addTerm("world", 3, 1);
        index.addTerm("world", 3, 2);
    }
};

// 1. TFIDF: Пустой запрос
TEST_F(RankingTest, TFIDF_EmptyQuery) {
    TFIDFStrategy strategy;
    query.terms = {};
    auto results = strategy.rank(query, index);
    EXPECT_TRUE(results.empty());
}

// 2. TFIDF: Термин, которого нет в индексе
TEST_F(RankingTest, TFIDF_NoMatchingTerms) {
    TFIDFStrategy strategy;
    query.terms = {"unknown"};
    auto results = strategy.rank(query, index);
    EXPECT_TRUE(results.empty());
}

// 3. TFIDF: Корректная сортировка по релевантности
TEST_F(RankingTest, TFIDF_SortingOrder) {
    TFIDFStrategy strategy;
    query.terms = {"world"};
    auto results = strategy.rank(query, index);

    ASSERT_EQ(results.size(), 2);
    // Документ 3 имеет "world" 3 раза, Документ 1 - 1 раз. Doc3 должен быть выше.
    EXPECT_EQ(results[0].doc.id, 3);
    EXPECT_EQ(results[1].doc.id, 1);
}

// 4. BM25: Пустой запрос
TEST_F(RankingTest, BM25_EmptyQuery) {
    BM25Strategy strategy;
    query.terms = {};
    auto results = strategy.rank(query, index);
    EXPECT_TRUE(results.empty());
}

// 5. BM25: Влияние изменения параметров (k1, b)
TEST_F(RankingTest, BM25_ParametersChangeScore) {
    BM25Strategy strategy1;
    BM25Strategy strategy2;
    strategy2.setParameters(2.0, 1.0); // Изменяем стандартные параметры коэффициентов

    query.terms = {"hello"};

    auto res1 = strategy1.rank(query, index);
    auto res2 = strategy2.rank(query, index);

    ASSERT_FALSE(res1.empty());
    ASSERT_FALSE(res2.empty());
    // Оценки релевантности должны отличаться из-за разных k1 и b
    EXPECT_NE(res1[0].score, res2[0].score);
}

// 6. Ranker: Корректное перенаправление стратегии
TEST_F(RankingTest, RankerDelegation) {
    Ranker ranker;
    ranker.setStrategy(std::make_unique<TFIDFStrategy>());
    query.terms = {"cplusplus"};

    auto results = ranker.rank(query, index);
    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0].doc.id, 2);
}

// 7. Ranker: Поведение без установленной стратегии
TEST_F(RankingTest, RankerNoStrategy) {
    Ranker ranker; // Стратегия nullptr
    query.terms = {"hello"};
    auto results = ranker.rank(query, index);
    EXPECT_TRUE(results.empty());
}