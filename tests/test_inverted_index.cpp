#include <gtest/gtest.h>
#include "InvertedIndex.h"

class InvertedIndexTest : public ::testing::Test {
protected:
    InvertedIndex index;
};

// 1. Добавление нового термина и проверка его наличия
TEST_F(InvertedIndexTest, AddAndRetrieveTerm) {
    index.addTerm("cplusplus", 1, 0);
    auto postings = index.getPostings("cplusplus");

    ASSERT_EQ(postings.size(), 1);
    EXPECT_EQ(postings[0].documentId, 1);
    EXPECT_EQ(postings[0].termFrequency, 1);
    EXPECT_EQ(postings[0].positions, std::vector<int>{0});
}

// 2. Добавление существующего термина в тот же документ (увеличение частоты)
TEST_F(InvertedIndexTest, TermFrequencyIncrements) {
    index.addTerm("hello", 1, 5);
    index.addTerm("hello", 1, 10);

    auto postings = index.getPostings("hello");
    ASSERT_EQ(postings.size(), 1);
    EXPECT_EQ(postings[0].termFrequency, 2);
    std::vector<int> expectedPositions = {5, 10};
    EXPECT_EQ(postings[0].positions, expectedPositions);
}

// 3. Добавление термина в разные документы
TEST_F(InvertedIndexTest, MultipleDocumentsForTerm) {
    index.addTerm("search", 1, 0);
    index.addTerm("search", 2, 0);

    auto postings = index.getPostings("search");
    EXPECT_EQ(postings.size(), 2);
}

// 4. Поиск несуществующего термина
TEST_F(InvertedIndexTest, NonExistentTermReturnsEmpty) {
    auto postings = index.getPostings("unknown");
    EXPECT_TRUE(postings.empty());
}

// 5. Работа с метаданными документов (Document)
TEST_F(InvertedIndexTest, AddAndRetrieveDocument) {
    Document doc;
    doc.id = 1;
    doc.title = "test.txt";
    doc.path = "/path/test.txt";
    doc.size = 1024;

    index.addDocument(1, doc);

    auto retrieved = index.getDocument(1);
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(retrieved.value().title, "test.txt");
    EXPECT_EQ(index.getDocumentCount(), 1);
}

// 6. Подсчет средней длины документа (AvgDocLength)
TEST_F(InvertedIndexTest, DocumentLengthAndAverage) {
    index.setDocLength(1, 10);
    index.setDocLength(2, 20);

    EXPECT_EQ(index.getDocLength(1), 10);
    EXPECT_DOUBLE_EQ(index.getAvgDocLength(), 15.0);
}

// 7. Очистка индекса
TEST_F(InvertedIndexTest, ClearRemovesEverything) {
    index.addTerm("test", 1, 0);
    index.setDocLength(1, 5);
    index.clear();

    EXPECT_TRUE(index.getPostings("test").empty());
    EXPECT_EQ(index.getDocumentCount(), 0);
    EXPECT_DOUBLE_EQ(index.getAvgDocLength(), 1.0); // По умолчанию возвращает 1.0 при пустом списке
}