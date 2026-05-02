#include "Stemmer.h"
#include <libstemmer.h> 

Stemmer::Stemmer(const std::string& language)
    : language_(language)
{
    stemmer = sb_stemmer_new(language.c_str(), nullptr);
    if (!stemmer) {
        throw std::runtime_error("Failed to create stemmer for language: " + language);
    }
}

Stemmer::~Stemmer() {
    if (stemmer) {
        sb_stemmer_delete(stemmer);
    }
}

Stemmer::Stemmer(Stemmer&& other) noexcept
    : stemmer(other.stemmer)
    , language_(std::move(other.language_))
{
    other.stemmer = nullptr;
}

Stemmer& Stemmer::operator=(Stemmer&& other) noexcept {
    if (this != &other) {
        if (stemmer) {
            sb_stemmer_delete(stemmer);
        }
        stemmer = other.stemmer;
        language_ = std::move(other.language_);
        other.stemmer = nullptr;
    }
    return *this;
}

std::string Stemmer::stem(const std::string& word) const {
    if (!stemmer || word.empty()) {
        return word;
    }

    const sb_symbol* stemmed = sb_stemmer_stem(
        stemmer,
        reinterpret_cast<const sb_symbol*>(word.c_str()),
        word.length()
    );

    if (!stemmed) {
        return word;
    }

    int len = sb_stemmer_length(stemmer);
    return std::string(reinterpret_cast<const char*>(stemmed), len);
}

std::vector<std::string> Stemmer::stem(const std::vector<std::string>& words) const {
    std::vector<std::string> result;
    result.reserve(words.size());
    for (const auto& w : words) {
        result.push_back(stem(w));
    }
    return result;
}