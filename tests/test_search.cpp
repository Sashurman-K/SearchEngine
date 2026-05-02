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
#include "QueryParser.h"
#include "Ranker.h"
#include "TFIDFStrategy.h"
#include "BM25Strategy.h"
#include "SearchEngine.h"

namespace fs = std::filesystem;

// ============ Вспомогательные функции ============

fs::path createSearchTestDir() {
    auto tmpDir = fs::temp_directory_path() / "search_ranking_test";
    fs::remove_all(tmpDir);
    fs::create_directories(tmpDir);

    // Документ 0: "cat dog cat"
    { std::ofstream f(tmpDir / "doc0.txt"); f << "cat dog cat"; }

    // Документ 1: "dog bird"
    { std::ofstream f(tmpDir / "doc1.txt"); f << "dog bird"; }

    // Документ 2: "cat cat cat bird"
    { std::ofstream f(tmpDir / "doc2.txt"); f << "cat cat cat bird"; }

    return tmpDir;
}

// ============ Тесты ============

void testQueryParser() {
    std::cout << "[TEST] QueryParser... ";
    QueryParser parser;

    QueryNode node = parser.parse("Hello World Cat");
    assert(node.terms.size() == 3);
    assert(node.terms[0] == "hello");
    assert(node.terms[1] == "world");
    assert(node.terms[2] == "cat");

    QueryNode empty = parser.parse("");
    assert(empty.terms.empty());

    QueryNode punct = parser.parse("  hello,   world!  ");
    assert(punct.terms.size() == 2);
    assert(punct.terms[0] == "hello");
    assert(punct.terms[1] == "world");

    std::cout << "PASSED\n";
}

void testSearchEngineWithTFIDF() {
    std::cout << "[TEST] SearchEngine + TF-IDF... ";
    auto tmpDir = createSearchTestDir();

    Indexer indexer;
    indexer.setProcessor(std::make_unique<BaseTextProcessor>());
    indexer.indexDirectory(tmpDir.string(), {});

    InvertedIndex index = indexer.getIndex(); // копия

    // Создаём Ranker с TF-IDF
    Ranker ranker;
    ranker.setStrategy(std::make_unique<TFIDFStrategy>());

    SearchEngine engine(std::move(index), std::move(ranker));

    // Поиск "cat"
    auto results = engine.search("cat");
    assert(results.size() == 2); // doc0 и doc2

    // doc2 (3 раза "cat") должен быть выше doc0 (2 раза "cat")
    assert(results[0].score > results[1].score);
    assert(results[0].doc.title == "doc2.txt");
    assert(results[1].doc.title == "doc0.txt");

    // Поиск "bird"
    results = engine.search("bird");
    assert(results.size() == 2); // doc1 и doc2

    // Поиск "cat dog"
    results = engine.search("cat dog");
    assert(!results.empty());
    // Проверяем только что оба документа с "cat" найдены
    // (у кого выше score — зависит от формулы, не привязываемся к порядку)
    bool hasDoc0 = false;
    bool hasDoc2 = false;
    for (const auto& r : results) {
        if (r.doc.title == "doc0.txt") hasDoc0 = true;
        if (r.doc.title == "doc2.txt") hasDoc2 = true;
    }
assert(hasDoc0);
assert(hasDoc2);

    fs::remove_all(tmpDir);
    std::cout << "PASSED\n";
}

void testSearchEngineWithBM25() {
    std::cout << "[TEST] SearchEngine + BM25... ";
    auto tmpDir = createSearchTestDir();

    Indexer indexer;
    indexer.setProcessor(std::make_unique<BaseTextProcessor>());
    indexer.indexDirectory(tmpDir.string(), {});

    InvertedIndex index = indexer.getIndex();

    Ranker ranker;
    ranker.setStrategy(std::make_unique<BM25Strategy>());

    SearchEngine engine(std::move(index), std::move(ranker));

    // Поиск "cat"
    auto results = engine.search("cat");
    assert(results.size() == 2); // doc0 и doc2

    // doc2 (3 "cat") выше, чем doc0 (2 "cat")
    assert(results[0].score > results[1].score);
    assert(results.size() == 2);

    // Поиск несуществующего терма
    results = engine.search("elephant");
    assert(results.empty());

    fs::remove_all(tmpDir);
    std::cout << "PASSED\n";
}

void testEmptySearch() {
    std::cout << "[TEST] Empty search... ";
    auto tmpDir = createSearchTestDir();

    Indexer indexer;
    indexer.setProcessor(std::make_unique<BaseTextProcessor>());
    indexer.indexDirectory(tmpDir.string(), {});

    InvertedIndex index = indexer.getIndex();

    Ranker ranker;
    ranker.setStrategy(std::make_unique<TFIDFStrategy>());

    SearchEngine engine(std::move(index), std::move(ranker));

    auto results = engine.search("");
    assert(results.empty());

    results = engine.search("   ");
    assert(results.empty());

    fs::remove_all(tmpDir);
    std::cout << "PASSED\n";
}

void testStrategySwitch() {
    std::cout << "[TEST] Strategy switch... ";
    auto tmpDir = createSearchTestDir();

    Indexer indexer;
    indexer.setProcessor(std::make_unique<BaseTextProcessor>());
    indexer.indexDirectory(tmpDir.string(), {});

    InvertedIndex index = indexer.getIndex();

    Ranker ranker;

    // Сначала TF-IDF
    ranker.setStrategy(std::make_unique<TFIDFStrategy>());
    SearchEngine engine1(index, std::move(ranker)); // копия index для первого двигателя
    auto results1 = engine1.search("cat");
    assert(!results1.empty());

    // Потом BM25 — нужен новый индекс и новый ранкер
    InvertedIndex index2 = indexer.getIndex();
    Ranker ranker2;
    ranker2.setStrategy(std::make_unique<BM25Strategy>());
    SearchEngine engine2(std::move(index2), std::move(ranker2));
    auto results2 = engine2.search("cat");
    assert(!results2.empty());

    // Оба находят doc2 выше doc0
    assert(results1[0].doc.title == "doc2.txt");
    assert(results2[0].doc.title == "doc2.txt");

    fs::remove_all(tmpDir);
    std::cout << "PASSED\n";
}

// ============ Main ============

int main() {
    std::cout << "=== Search Tests ===\n\n";

    testQueryParser();
    testSearchEngineWithTFIDF();
    testSearchEngineWithBM25();
    testEmptySearch();
    testStrategySwitch();

    std::cout << "\n=== ALL SEARCH TESTS PASSED ===\n";
    return 0;
}