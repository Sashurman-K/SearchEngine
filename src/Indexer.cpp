#include "Indexer.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>

void Indexer::setProcessor(std::unique_ptr<ITextProcessor> p) {
    processor = std::move(p);
}



namespace fs = std::filesystem;

void Indexer::indexDirectory(
    const std::string& path,
    const std::vector<std::string>& excludedPaths)
{
    std::cout << "Indexing directory: " << path << std::endl;

    if (!fs::exists(path) || !fs::is_directory(path)) {
        std::cerr << "Error: invalid directory path: " << path << std::endl;
        return;
    }

    // Лямбда для проверки: нужно ли исключить путь
    auto isExcluded = [&excludedPaths](const fs::path& p) -> bool {
        return std::any_of(
            excludedPaths.begin(),
            excludedPaths.end(),
            [&p](const std::string& excluded) {
                // Точное совпадение имени файла/директории
                if (p.filename() == excluded) return true;
                // Проверка полного пути (содержит исключённую подстроку)
                if (p.string().find(excluded) != std::string::npos) return true;
                return false;
            }
        );
    };

    // Рекурсивный обход
    for (auto it = fs::recursive_directory_iterator(path);
     it != fs::recursive_directory_iterator();
     ++it)
{
    const auto& entryPath = it->path();

    // Пропускаем исключённые директории и файлы
    if (isExcluded(entryPath)) {
        if (it->is_directory()) {
            std::cout << "  Skipping excluded directory: " << entryPath << std::endl;
            // Ключевой момент: отключаем рекурсию внутрь исключённой директории
            it.disable_recursion_pending();
        } else {
            std::cout << "  Skipping excluded file: " << entryPath << std::endl;
        }
        continue;
    }

    // Индексируем только обычные файлы
    if (it->is_regular_file()) {
        indexFile(entryPath);
    }
}
}

void Indexer::indexFile(const fs::path& filePath) {
    std::cout << "  Indexing file: " << filePath << std::endl;

    // Генерируем уникальный ID документа (можно заменить на свой механизм)
    static int nextDocId = 1;
    int docId = nextDocId++;

    // Читаем содержимое файла
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "  Warning: cannot open file: " << filePath << std::endl;
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    // Токенизируем (упрощённо — разбиваем по пробелам и знакам препинания)
    std::vector<std::string> tokens = tokenize(content);

    // Применяем цепочку обработчиков текста (StopWordsDecorator и т.д.)
    if (processor) {
        tokens = processor->process(tokens);
    }

    // Добавляем токены в инвертированный индекс с позициями
    for (int pos = 0; pos < static_cast<int>(tokens.size()); ++pos) {
        if (!tokens[pos].empty()) {
            index.addTerm(tokens[pos], docId, pos);
        }
    }

    // Создаём метаинформацию о документе
    Document doc;
    doc.id = docId;
    doc.title =filePath.filename();
    doc.path = filePath.string();
    doc.size = fs::file_size(filePath);

    index.addDocument(docId, doc);
}

std::vector<std::string> Indexer::tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    std::string currentToken;

    for (char c : text) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            currentToken += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        } else {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
        }
    }

    if (!currentToken.empty()) {
        tokens.push_back(currentToken);
    }

    return tokens;
}
const InvertedIndex& Indexer::getIndex() const {
    return index;
}