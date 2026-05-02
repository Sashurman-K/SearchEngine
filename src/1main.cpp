#include <iostream>
#include "SearchEngine.h"
#include "Indexer.h"

// int main() {
//     std::cout << "--- Initialization ---\n";

//     // 1. Создаем Индексатор и настраиваем конвейер Декораторов текста
//     Indexer indexer;
//     auto textPipeline = std::make_unique<StemmingDecorator>(
//         std::make_unique<StopWordsDecorator>(
//             std::make_unique<BaseTextProcessor>()
//         )
//     );
//     indexer.setProcessor(std::move(textPipeline));
//     indexer.indexDirectory("/var/data/docs");

//     // 2. Настраиваем Ранжировщик с паттерном Стратегия
//     Ranker ranker;
//     ranker.setStrategy(std::make_unique<BM25Strategy>());

//     // 3. Собираем поисковый движок
//     SearchEngine engine(indexer.getIndex(), std::move(ranker));

//     // 4. Выполняем поиск
//     std::cout << "\n--- Searching ---\n";
//     auto results = engine.search("example query");

//     for (const auto& res : results) {
//         std::cout << "Found: " << res.doc.title << " (Score: " << res.score << ")\n";
//     }

//     return 0;
// }