#pragma once

#include "TextDecorator.h"
#include "Stemmer.h"
#include <memory>

class StemmingDecorator : public TextDecorator {
public:
    // language: "en", "ru", "de", ...
    StemmingDecorator(
        std::unique_ptr<ITextProcessor> processor,
        const std::string& language
    );

    std::vector<std::string> process(std::vector<std::string> tokens) const override;

private:
    Stemmer stemmer;
};