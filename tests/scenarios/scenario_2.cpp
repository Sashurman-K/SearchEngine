#include <iostream>
#include <cassert>
#include <memory>
#include "InvertedIndex.h"
#include "Ranker.h"
#include "TFIDFStrategy.h"
#include "BM25Strategy.h"
#include "QueryParser.h"
//Сценарий использования различны стратегий ранжирования
void runScenario2() {
    std::cout << "[SCENARIO 2] Starting Dynamic Strategy Switching Test...\n";

    // 1. Ручное наполнение индекса контролируемыми данными
    InvertedIndex index;

    Document d1{1, "short_doc.txt", "/path1", 10};
    Document d2{2, "very_long_document_about_programming.txt", "/path2", 500};

    index.addDocument(1, d1); index.setDocLength(1, 3);   // Короткий: "c++ code smart"
    index.addDocument(2, d2); index.setDocLength(2, 200); // Длинный спам-текст, где "c++" встречается пару раз

    index.addTerm("cplusplus", 1, 0);
    index.addTerm("cplusplus", 2, 10);
    index.addTerm("cplusplus", 2, 50);

    QueryParser parser;
    QueryNode query = parser.parse("cplusplus");

    // 2. Тестируем TF-IDF
    Ranker ranker;
    ranker.setStrategy(std::make_unique<TFIDFStrategy>());
    auto tfidfResults = ranker.rank(query, index);

    std::cout << "TF-IDF Results:\n";
    for (const auto& r : tfidfResults) {
        std::cout << "  Doc ID: " << r.doc.id << " -> Score: " << r.score << "\n";
    }

    // 3. Переключаемся на BM25 (он штрафует за длину документа)
    ranker.setStrategy(std::make_unique<BM25Strategy>());
    auto bm25Results = ranker.rank(query, index);

    std::cout << "BM25 Results (with length penalty):\n";
    for (const auto& r : bm25Results) {
        std::cout << "  Doc ID: " << r.doc.id << " -> Score: " << r.score << "\n";
    }

    // Проверяем, что оценки шкал отличаются
    assert(!tfidfResults.empty() && !bm25Results.empty());
    assert(tfidfResults[0].score != bm25Results[0].score && "Scenario 2 Failed: Scores must differ between strategies!");

    std::cout << "[SCENARIO 2] PASSED SUCCESSFULLY!\n\n";
}

int main() {
    runScenario2();
    return 0;
}