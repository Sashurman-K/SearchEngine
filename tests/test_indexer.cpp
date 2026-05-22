#include <gtest/gtest.h>
#include "Indexer.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

class IndexerTest : public ::testing::Test {
protected:
    std::string testDir = "test_data_dir";

    void SetUp() override {
        // Создаем временную тестовую директорию и файлы перед каждым тестом
        fs::create_directory(testDir);

        std::ofstream(testDir + "/file1.txt") << "hello world";
        std::ofstream(testDir + "/file2.txt") << "C++ programming engine";

        fs::create_directory(testDir + "/exclude_me");
        std::ofstream(testDir + "/exclude_me/file3.txt") << "secret data";
    }

    void TearDown() override {
        // Удаляем временную папку после завершения теста
        fs::remove_all(testDir);
    }
};

// 1. Индексация директории без исключений
TEST_F(IndexerTest, IndexValidDirectory) {
    Indexer indexer;
    indexer.indexDirectory(testDir, {});

    const auto& index = indexer.getIndex();
    EXPECT_EQ(index.getDocumentCount(), 3); // file1, file2, file3
    EXPECT_FALSE(index.getPostings("hello").empty());
}

// 2. Исключение конкретного файла из индексации
TEST_F(IndexerTest, ExcludeSpecificFile) {
    Indexer indexer;
    indexer.indexDirectory(testDir, {"file2.txt"});

    const auto& index = indexer.getIndex();
    EXPECT_EQ(index.getDocumentCount(), 2);
    EXPECT_TRUE(index.getPostings("programming").empty()); // Было только в file2.txt
}

// 3. Исключение поддиректории
TEST_F(IndexerTest, ExcludeDirectory) {
    Indexer indexer;
    indexer.indexDirectory(testDir, {"exclude_me"});

    const auto& index = indexer.getIndex();
    EXPECT_EQ(index.getDocumentCount(), 2);
    EXPECT_TRUE(index.getPostings("secret").empty()); // Было только внутри папки exclude_me
}

// 4. Попытка индексации несуществующей директории
TEST_F(IndexerTest, IndexNonExistentDirectory) {
    Indexer indexer;
    indexer.indexDirectory("some_fake_directory_123", {});
    EXPECT_EQ(indexer.getIndex().getDocumentCount(), 0);
}

// 5. Проверка работы токенизации (пробелы и пунктуация)
TEST_F(IndexerTest, TokenizationPunctuation) {
    std::ofstream(testDir + "/complex.txt") << "Hello, world! (C++20)";
    Indexer indexer;
    // Оставляем только complex.txt
    indexer.indexDirectory(testDir, {"file1.txt", "file2.txt", "exclude_me"});

    const auto& index = indexer.getIndex();
    // Токенизатор убирает знаки препинания, переводит в нижний регистр
    EXPECT_FALSE(index.getPostings("hello").empty());
    EXPECT_FALSE(index.getPostings("world").empty());
    EXPECT_FALSE(index.getPostings("c").empty());
    EXPECT_FALSE(index.getPostings("20").empty());
}