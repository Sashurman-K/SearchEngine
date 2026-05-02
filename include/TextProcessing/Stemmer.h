#pragma once

#include <string>
#include <memory>
#include <stdexcept>
#include <vector>
// Forward declaration для libstemmer
struct sb_stemmer;

class Stemmer {
public:
    // language: "en", "ru", "de", "fr", "es" и т.д.
    explicit Stemmer(const std::string& language);
    ~Stemmer();

    // Запрет копирования (владеет C-указателем)
    Stemmer(const Stemmer&) = delete;
    Stemmer& operator=(const Stemmer&) = delete;

    // Перемещение
    Stemmer(Stemmer&& other) noexcept;
    Stemmer& operator=(Stemmer&& other) noexcept;

    // Стемминг одного слова
    std::string stem(const std::string& word) const;

    // Стемминг вектора слов
    std::vector<std::string> stem(const std::vector<std::string>& words) const;

private:
    struct sb_stemmer* stemmer;
    std::string language_;
};
