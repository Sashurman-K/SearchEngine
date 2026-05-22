#include <iostream>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <memory>

#include "Indexer.h"
#include "InvertedIndex.h"
#include "SearchEngine.h"
#include "Ranker.h"
#include "TFIDFStrategy.h"
#include "BaseTextProcessor.h"
#include "StopWordsDecorator.h"
#include "StemmingDecorator.h"

namespace fs = std::filesystem;
// Сценарий полной работы программы и модулей
void runScenario1() {
    std::cout << "[SCENARIO 1] Starting End-to-End Search Pipeline Test...\n";

    // 1. Подготовка окружения
    std::string testDir = "scenario_1_data";
    fs::create_directory(testDir);

    std::ofstream(testDir + "/doc1.txt") << "The quick brown fox jumps over the lazy dog.";
    std::ofstream(testDir + "/doc2.txt") << "Foxes love jumping and running in the brown woods.";

    // 2. Настройка цепочки обработки текста (Лингвистика)
    auto baseProcessor = std::make_unique<BaseTextProcessor>();
    auto stopWordsProcessor = std::make_unique<StopWordsDecorator>(std::move(baseProcessor), "english");
    auto fullChain = std::make_unique<StemmingDecorator>(std::move(stopWordsProcessor), "english");

    // 3. Индексация
    Indexer indexer;
    indexer.setProcessor(std::move(fullChain));
    indexer.indexDirectory(testDir, {});

    // 4. Инициализация поискового движка
    InvertedIndex index = indexer.getIndex();
    Ranker ranker;
    ranker.setStrategy(std::make_unique<TFIDFStrategy>());
    SearchEngine engine(std::move(index), std::move(ranker));

    // 5. Поисковый запрос (содержит стоп-слова "the", разные формы "fox/foxes", "jumping/jumps")
    std::string query = "The brown foxes jumping";
    auto results = engine.search(query);

    // 6. Валидация результатов
    std::cout << "Search results for: '" << query << "'\n";
    for (const auto& res : results) {
        std::cout << "  - Document: " << res.doc.title << " | Score: " << res.score << "\n";
    }

    // Проверяем, что оба документа найдены, так как корни слов совпали после стемминга
    assert(results.size() == 2 && "Scenario 1 Failed: Both documents should match the stemmed query!");

    // Очистка
    fs::remove_all(testDir);
    std::cout << "[SCENARIO 1] PASSED SUCCESSFULLY!\n\n";
}

int main() {
    try {
        runScenario1();
    } catch (const std::exception& e) {
        std::cerr << "Scenario 1 crashed with exception: " << e.what() << "\n";
        return 1;
    }
    return 0;
}