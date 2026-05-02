#pragma once

#include <string>
#include <unordered_set>
#include <filesystem>

class StopWordsConfig {
public:
    // Загружает стоп-слова для указанного языка из JSON-файла
    static std::unordered_set<std::string> load(
        const std::string& language,
        const std::string& configPath = ""
    );

private:
    static std::filesystem::path findConfigFile(const std::string& configPath);
};