#include "StopWordsDecorator.h"
#include "StopWordsConfig.h"
#include <algorithm>

StopWordsDecorator::StopWordsDecorator(
    std::unique_ptr<ITextProcessor> processor,
    const std::string& language,
    const std::string& configPath)
    : TextDecorator(std::move(processor))
    , stopWords(StopWordsConfig::load(language, configPath))
{}

std::vector<std::string> StopWordsDecorator::process(std::vector<std::string> tokens) const {
    tokens.erase(
        std::remove_if(tokens.begin(), tokens.end(),
            [this](const std::string& token) {
                return stopWords.find(token) != stopWords.end();
            }
        ),
        tokens.end()
    );

    if (wrapped) {
        return wrapped->process(std::move(tokens));
    }
    return tokens;
}