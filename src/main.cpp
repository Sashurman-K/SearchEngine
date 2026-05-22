#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <filesystem>

#include "Indexer.h"
#include "InvertedIndex.h"
#include "SearchEngine.h"
#include "QueryParser.h"
#include "Ranker.h"
#include "TFIDFStrategy.h"
#include "BM25Strategy.h"
#include "BaseTextProcessor.h"
#include "StopWordsDecorator.h"
#include "StemmingDecorator.h"

namespace fs = std::filesystem;

//  Глобальное состояние
struct AppState {
    InvertedIndex index;
    bool hasIndex = false;
    std::string currentStrategy = "tfidf";
};

//  Команды

void cmdHelp() {
    std::cout << "\n"
              << "Available commands:\n"
              << "  index <path> [--exclude <dir> ...]  — index directory\n"
              << "  search <query>                       — search documents\n"
              << "  strategy <tfidf|bm25>               — switch ranking strategy\n"
              << "  info                                 — show index info\n"
              << "  clear                                — clear index\n"
              << "  help                                 — this message\n"
              << "  exit                                 — quit\n"
              << "\n";
}

void cmdIndex(AppState& state, const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Error: path required. Usage: index <path> [--exclude ...]\n";
        return;
    }

    std::string dirPath = args[0];
    if (!fs::exists(dirPath) || !fs::is_directory(dirPath)) {
        std::cerr << "Error: '" << dirPath << "' is not a valid directory.\n";
        return;
    }

    std::vector<std::string> excluded;
    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i] == "--exclude" && i + 1 < args.size()) {
            excluded.push_back(args[++i]);
        }
    }

    // Собираем цепочку обработки
    auto base = std::make_unique<BaseTextProcessor>();
    auto stopWords = std::make_unique<StopWordsDecorator>(
        std::move(base), "english", "stopwords_config.json");
    auto stemmer = std::make_unique<StemmingDecorator>(
        std::move(stopWords), "en");

    Indexer indexer;
    indexer.setProcessor(std::move(stemmer));
    indexer.indexDirectory(dirPath, excluded);

    state.index = indexer.getIndex();
    state.hasIndex = true;

    std::cout << "Done. Indexed " << state.index.getDocumentCount() << " documents.\n";
}

void cmdSearch(AppState& state, const std::vector<std::string>& args) {
    if (!state.hasIndex) {
        std::cerr << "No index. Use 'index <path>' first.\n";
        return;
    }

    if (args.empty()) {
        std::cerr << "Error: query required. Usage: search <words>\n";
        return;
    }

    std::string query;
    for (size_t i = 0; i < args.size(); ++i) {
        if (!query.empty()) query += " ";
        query += args[i];
    }

    // === ДИАГНОСТИКА ===
    std::cout << "DEBUG: query = \"" << query << "\"\n";
    std::cout << "DEBUG: index has " << state.index.getDocumentCount() << " documents\n";

    QueryParser parser;
    QueryNode node = parser.parse(query);
    std::cout << "DEBUG: parsed terms: ";
    for (const auto& t : node.terms) {
        std::cout << "'" << t << "' ";
    }
    std::cout << "\n";

    // Проверяем, есть ли термы в индексе
    for (const auto& term : node.terms) {
        auto postings = state.index.getPostings(term);
        std::cout << "DEBUG: term '" << term << "' has " << postings.size() << " postings\n";
    }


    Ranker ranker;
    if (state.currentStrategy == "bm25") {
        ranker.setStrategy(std::make_unique<BM25Strategy>());
    } else {
        ranker.setStrategy(std::make_unique<TFIDFStrategy>());
    }

    SearchEngine engine(state.index, std::move(ranker));
    auto results = engine.search(query);

    std::cout << "\nResults for \"" << query << "\"";
    std::cout << " (strategy: " << state.currentStrategy << "):\n";
    std::cout << std::string(60, '-') << "\n";

    if (results.empty()) {
        std::cout << "  Nothing found.\n";
    } else {
        size_t shown = std::min(results.size(), size_t(10));
        for (size_t i = 0; i < shown; ++i) {
            std::cout << "  " << (i + 1) << ". "
                      << results[i].doc.title << "\n"
                      << "     score: " << results[i].score << "\n"
                      << "     path:  " << results[i].doc.path << "\n";
            if (i < shown - 1) std::cout << "\n";
        }
    }
    std::cout << std::string(60, '-') << "\n";
}

void cmdStrategy(AppState& state, const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Current strategy: " << state.currentStrategy << "\n";
        std::cout << "Available: tfidf, bm25\n";
        return;
    }

    std::string strat = args[0];
    if (strat == "tfidf" || strat == "bm25") {
        state.currentStrategy = strat;
        std::cout << "Strategy set to: " << strat << "\n";
    } else {
        std::cerr << "Unknown strategy. Use 'tfidf' or 'bm25'.\n";
    }
}

void cmdInfo(const AppState& state) {
    if (!state.hasIndex) {
        std::cout << "No index loaded.\n";
        return;
    }

    std::cout << "\n"
              << "Index info:\n"
              << "  Documents: " << state.index.getDocumentCount() << "\n"
              << "  Strategy:  " << state.currentStrategy << "\n";

    // Показываем несколько документов
    const auto& docs = state.index.getDocuments();
    std::cout << "  Files:\n";
    int count = 0;
    for (const auto& [id, doc] : docs) {
        std::cout << "    [" << id << "] " << doc.title << "\n";
        if (++count >= 5) {
            if (docs.size() > 5) {
                std::cout << "    ... and " << (docs.size() - 5) << " more.\n";
            }
            break;
        }
    }
    std::cout << "\n";
}

void cmdClear(AppState& state) {
    state.index.clear();
    state.hasIndex = false;
    std::cout << "Index cleared.\n";
}

//  Main 

int main() {
    AppState state;

    std::cout << "========================================\n";
    std::cout << "  Search Engine CLI\n";
    std::cout << "  Type 'help' for commands, 'exit' to quit.\n";
    std::cout << "========================================\n\n";

    std::string line;

    while (true) {
        std::cout << "> " << std::flush;

        if (!std::getline(std::cin, line)) {
            break; // Ctrl+D
        }

        // Парсим строку
        std::istringstream stream(line);
        std::string command;
        stream >> command;

        if (command.empty()) {
            continue;
        }

        // Собираем аргументы
        std::vector<std::string> args;
        std::string arg;
        while (stream >> arg) {
            args.push_back(arg);
        }

        // Обрабатываем команду
        if (command == "exit" || command == "quit") {
            std::cout << "Bye.\n";
            break;
        } else if (command == "help") {
            cmdHelp();
        } else if (command == "index") {
            cmdIndex(state, args);
        } else if (command == "search") {
            cmdSearch(state, args);
        } else if (command == "strategy") {
            cmdStrategy(state, args);
        } else if (command == "info") {
            cmdInfo(state);
        } else if (command == "clear") {
            cmdClear(state);
        } else {
            std::cerr << "Unknown command: " << command << ". Type 'help'.\n";
        }
    }

    return 0;
}