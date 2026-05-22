#include <gtest/gtest.h>
#include "QueryParser.h"

class QueryParserTest : public ::testing::Test {
protected:
    QueryParser parser;
};

// Тест 1: Пустой запрос
TEST_F(QueryParserTest, EmptyQuery) {
    QueryNode result = parser.parse("");
    EXPECT_TRUE(result.terms.empty());
}

// Тест 2: Обычный корректный запрос в нижнем регистре
TEST_F(QueryParserTest, SimpleLowercaseQuery) {
    QueryNode result = parser.parse("hello world search");
    std::vector<std::string> expected = {"hello", "world", "search"};
    EXPECT_EQ(result.terms, expected);
}

// Тест 3: Проверка приведения к нижнему регистру (Case Insensitivity)
TEST_F(QueryParserTest, CaseInsensitivity) {
    QueryNode result = parser.parse("HeLLo WoRlD");
    std::vector<std::string> expected = {"hello", "world"};
    EXPECT_EQ(result.terms, expected);
}

// Тест 4: Очистка от знаков препинания по краям слов
TEST_F(QueryParserTest, PunctuationRemoval) {
    QueryNode result = parser.parse("hello, world! (search)...");
    std::vector<std::string> expected = {"hello", "world", "search"};
    EXPECT_EQ(result.terms, expected);
}

// Тест 5: Обработка множественных пробелов и символов табуляции
TEST_F(QueryParserTest, MultipleSpacesAndTabs) {
    QueryNode result = parser.parse("hello   \t world  \n  engine");
    std::vector<std::string> expected = {"hello", "world", "engine"};
    EXPECT_EQ(result.terms, expected);
}

// Тест 6: Строка, состоящая только из знаков препинания и пробелов
TEST_F(QueryParserTest, OnlyPunctuationAndSpaces) {
    QueryNode result = parser.parse("!!! ??? ,,,   ...");
    EXPECT_TRUE(result.terms.empty());
}

