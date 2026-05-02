#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>

#include "Types.h"
#include "InvertedIndex.h"
#include "Indexer.h"
#include "BaseTextProcessor.h"

namespace fs = std::filesystem;

void testBasicIndexing() {
    std::cout << "[TEST] Basic indexing... ";
    auto tmpDir = fs::temp_directory_path() / "search_test";
    fs::remove_all(tmpDir);
    fs::create_directories(tmpDir);
    { std::ofstream f(tmpDir / "doc1.txt"); f << "hello world"; }
    { std::ofstream f(tmpDir / "doc2.txt"); f << "hello cat"; }

    Indexer indexer;
    indexer.setProcessor(std::make_unique<BaseTextProcessor>());
    indexer.indexDirectory(tmpDir.string(), {});
    const auto& index = indexer.getIndex();

    assert(index.getPostings("hello").size() == 2);
    assert(index.getPostings("world").size() == 1);
    assert(index.getDocumentCount() == 2);

    fs::remove_all(tmpDir);
    std::cout << "PASSED\n";
}

void testEmptyDirectory() {
    std::cout << "[TEST] Empty directory... ";
    auto tmpDir = fs::temp_directory_path() / "search_empty";
    fs::remove_all(tmpDir);
    fs::create_directories(tmpDir);

    Indexer indexer;
    indexer.setProcessor(std::make_unique<BaseTextProcessor>());
    indexer.indexDirectory(tmpDir.string(), {});
    const auto& index = indexer.getIndex();

    assert(index.getDocumentCount() == 0);

    fs::remove_all(tmpDir);
    std::cout << "PASSED\n";
}

void testNonexistentDirectory() {
    std::cout << "[TEST] Nonexistent directory... ";
    Indexer indexer;
    indexer.setProcessor(std::make_unique<BaseTextProcessor>());
    indexer.indexDirectory("/nonexistent/path", {});
    const auto& index = indexer.getIndex();

    assert(index.getDocumentCount() == 0);
    std::cout << "PASSED\n";
}

void testTermFrequencyAndPositions() {
    std::cout << "[TEST] Term frequency and positions... ";
    auto tmpDir = fs::temp_directory_path() / "search_tf";
    fs::remove_all(tmpDir);
    fs::create_directories(tmpDir);
    { std::ofstream f(tmpDir / "doc.txt"); f << "cat cat dog cat"; }

    Indexer indexer;
    indexer.setProcessor(std::make_unique<BaseTextProcessor>());
    indexer.indexDirectory(tmpDir.string(), {});
    const auto& index = indexer.getIndex();

    auto postings = index.getPostings("cat");
    assert(postings.size() == 1);
    assert(postings[0].termFrequency == 3);
    assert(postings[0].positions == std::vector<int>({0, 1, 3}));

    fs::remove_all(tmpDir);
    std::cout << "PASSED\n";
}

void testExcludedPaths() {
    std::cout << "[TEST] Excluded paths... ";
    auto tmpDir = fs::temp_directory_path() / "search_excl";
    fs::remove_all(tmpDir);
    fs::create_directories(tmpDir);
    fs::create_directories(tmpDir / "secret");
    { std::ofstream f(tmpDir / "doc.txt"); f << "hello"; }
    { std::ofstream f(tmpDir / "secret/hidden.txt"); f << "hidden hello"; }

    Indexer indexer;
    indexer.setProcessor(std::make_unique<BaseTextProcessor>());
    indexer.indexDirectory(tmpDir.string(), {"secret"});
    const auto& index = indexer.getIndex();

    assert(index.getDocumentCount() == 1);         // только doc.txt
    assert(index.getPostings("hidden").empty()); // из secret/hidden.txt не попал
    assert(index.getPostings("hello").size() == 1);

    fs::remove_all(tmpDir);
    std::cout << "PASSED\n";
}

int main() {
    std::cout << "=== Indexer Tests ===\n\n";
    testBasicIndexing();
    testEmptyDirectory();
    testNonexistentDirectory();
    testTermFrequencyAndPositions();
    testExcludedPaths();
    std::cout << "\n=== ALL TESTS PASSED ===\n";
    return 0;
}
