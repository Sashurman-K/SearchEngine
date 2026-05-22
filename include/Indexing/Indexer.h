#pragma once
#include "InvertedIndex.h"
#include "ITextProcessor.h"
#include <memory>
#include <string>
#include <vector>
#include <filesystem>


class Indexer {
private:
    InvertedIndex index;
    std::unique_ptr<ITextProcessor> processor;
    void indexFile(const std::filesystem::path& path);
    std::vector<std::string> tokenize(const std::string& text) const;
public:
    void setProcessor(std::unique_ptr<ITextProcessor> p);
    void indexDirectory(const std::string& path, const std::vector<std::string>& excludedPaths);
    const InvertedIndex& getIndex() const;
};