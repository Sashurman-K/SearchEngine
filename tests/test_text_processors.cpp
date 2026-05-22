#include <gtest/gtest.h>
#include <memory>
#include "BaseTextProcessor.h"
#include "StopWordsDecorator.h"
#include "StemmingDecorator.h"


TEST(BaseTextProcessorTest, EmptyInput) {
    BaseTextProcessor processor;
    std::vector<std::string> tokens = {};
    EXPECT_TRUE(processor.process(tokens).empty());
}

TEST(BaseTextProcessorTest, ReturnsAsIs) {
    BaseTextProcessor processor;
    std::vector<std::string> tokens = {"test", "words", "list"};
    EXPECT_EQ(processor.process(tokens), tokens);
}

TEST(BaseTextProcessorTest, KeepsDuplicates) {
    BaseTextProcessor processor;
    std::vector<std::string> tokens = {"word", "word", "word"};
    EXPECT_EQ(processor.process(tokens), tokens);
}

TEST(BaseTextProcessorTest, SingleToken) {
    BaseTextProcessor processor;
    std::vector<std::string> tokens = {"data"};
    EXPECT_EQ(processor.process(tokens), tokens);
}

TEST(BaseTextProcessorTest, HandlesSpecialCharacters) {
    BaseTextProcessor processor;
    std::vector<std::string> tokens = {"@#$", "123", "c++"};
    EXPECT_EQ(processor.process(tokens), tokens);
}


class StopWordsDecoratorTest : public ::testing::Test {
protected:
    std::unique_ptr<StopWordsDecorator> decorator;
    void SetUp() override {
        auto base = std::make_unique<BaseTextProcessor>();
        // Передаем путь к тестовому или стандартному конфигурационному файлу
        decorator = std::make_unique<StopWordsDecorator>(std::move(base), "english", "stopwords_config.json");
    }
};

TEST_F(StopWordsDecoratorTest, NoStopWordsPassed) {
    std::vector<std::string> tokens = {"search", "engine", "project"};
    EXPECT_EQ(decorator->process(tokens), tokens);
}

TEST_F(StopWordsDecoratorTest, OnlyStopWordsPassed) {
    std::vector<std::string> tokens = {"the", "is", "at"};
    EXPECT_TRUE(decorator->process(tokens).empty());
}

TEST_F(StopWordsDecoratorTest, MixedTokensFiltering) {
    std::vector<std::string> tokens = {"the", "search", "is", "good", "at", "coding"};
    std::vector<std::string> expected = {"search", "good", "coding"};
    EXPECT_EQ(decorator->process(tokens), expected);
}

TEST_F(StopWordsDecoratorTest, EmptyInputVector) {
    std::vector<std::string> tokens = {};
    EXPECT_TRUE(decorator->process(tokens).empty());
}

TEST_F(StopWordsDecoratorTest, DuplicateStopWordsRemoved) {
    std::vector<std::string> tokens = {"the", "the", "search", "the"};
    std::vector<std::string> expected = {"search"};
    EXPECT_EQ(decorator->process(tokens), expected);
}

TEST_F(StopWordsDecoratorTest, CaseSensitivityCheck) {
    std::vector<std::string> tokens = {"The", "search"};
    std::vector<std::string> expected = {"The", "search"};
    EXPECT_EQ(decorator->process(tokens), expected);
}


class StemmingDecoratorTest : public ::testing::Test {
protected:
    std::unique_ptr<StemmingDecorator> decorator;
    void SetUp() override {
        auto base = std::make_unique<BaseTextProcessor>();
        decorator = std::make_unique<StemmingDecorator>(std::move(base), "en");
    }
};

TEST_F(StemmingDecoratorTest, EmptyTokens) {
    EXPECT_TRUE(decorator->process({}).empty());
}

TEST_F(StemmingDecoratorTest, StemmingPlurals) {
    std::vector<std::string> tokens = {"cats", "dogs", "engines"};
    std::vector<std::string> expected = {"cat", "dog", "engin"}; // Алгоритм Портера стеммит "engines" в "engin"
    EXPECT_EQ(decorator->process(tokens), expected);
}

TEST_F(StemmingDecoratorTest, StemmingVerbs) {
    std::vector<std::string> tokens = {"running", "walked", "searches"};
    std::vector<std::string> expected = {"run", "walk", "search"};
    EXPECT_EQ(decorator->process(tokens), expected);
}

TEST_F(StemmingDecoratorTest, UnchangedWords) {
    std::vector<std::string> tokens = {"code", "test", "root"};
    EXPECT_EQ(decorator->process(tokens), tokens);
}

TEST_F(StemmingDecoratorTest, ComplexChainVerification) {
    // Тест на полную матрешку: Base -> StopWords -> Stemming
    auto base = std::make_unique<BaseTextProcessor>();
    auto stop = std::make_unique<StopWordsDecorator>(std::move(base), "english", "stopwords_config.json");
    auto fullChain = std::make_unique<StemmingDecorator>(std::move(stop), "en");

    std::vector<std::string> tokens = {"the", "cats", "are", "running", "at", "night"};
    // "the", "are", "at" — стоп-слова (удаляются), остальные стеммятся
    std::vector<std::string> expected = {"cat", "run", "night"};
    EXPECT_EQ(fullChain->process(tokens), expected);
}