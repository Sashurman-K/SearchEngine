#pragma once

#include "TextDecorator.h"
#include <unordered_set>
#include <string>

class StopWordsDecorator : public TextDecorator {
public:
    // language: "english", "russian", ...
    StopWordsDecorator(
        std::unique_ptr<ITextProcessor> processor,
        const std::string& language,
        const std::string& configPath = ""
    );

    std::vector<std::string> process(std::vector<std::string> tokens) const override;

private:
    std::unordered_set<std::string> stopWords;
};