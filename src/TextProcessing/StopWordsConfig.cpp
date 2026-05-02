#include "StopWordsConfig.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <nlohmann/json.hpp> 

using json = nlohmann::json;

std::unordered_set<std::string> StopWordsConfig::load(
    const std::string& language,
    const std::string& configPath)
{
    std::unordered_set<std::string> stopWords;
    auto path = findConfigFile(configPath);

    if (path.empty() || !std::filesystem::exists(path)) {
        std::cerr << "Warning: stopwords config not found at " << path
                  << ". Using empty set.\n";
        return stopWords;
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Warning: cannot open stopwords config: " << path << "\n";
        return stopWords;
    }

    try {
        json config = json::parse(file);

        if (!config.contains(language)) {
            std::cerr << "Warning: language '" << language
                      << "' not found in stopwords config.\n";
            return stopWords;
        }

        for (const auto& word : config[language]) {
            stopWords.insert(word.get<std::string>());
        }

        std::cout << "Loaded " << stopWords.size() << " stopwords for language '"
                  << language << "' from " << path << "\n";
    }
    catch (const json::exception& e) {
        std::cerr << "Error parsing stopwords config: " << e.what() << "\n";
    }

    return stopWords;
}

std::filesystem::path StopWordsConfig::findConfigFile(const std::string& configPath) {
    // 1. Передан явный путь
    if (!configPath.empty()) {
        return configPath;
    }

    // 2. Переменная окружения
    const char* envPath = std::getenv("STOPWORDS_CONFIG_PATH");
    if (envPath && std::filesystem::exists(envPath)) {
        return envPath;
    }

    // 3. Рядом с исполняемым файлом
    std::error_code ec;
    auto exePath = std::filesystem::canonical("/proc/self/exe", ec);
    if (!ec) {
        auto candidate = exePath.parent_path() / "stopwords_config.json";
        if (std::filesystem::exists(candidate)) {
            return candidate;
        }
    }

    // 4. Текущая рабочая директория
    auto candidate = std::filesystem::current_path() / "stopwords_config.json";
    if (std::filesystem::exists(candidate)) {
        return candidate;
    }

    return {};
}