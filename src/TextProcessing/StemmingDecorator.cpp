#include "StemmingDecorator.h"
#include <algorithm>

StemmingDecorator::StemmingDecorator(
    std::unique_ptr<ITextProcessor> processor,
    const std::string& language)
    : TextDecorator(std::move(processor))
    , stemmer(language)
{}

std::vector<std::string> StemmingDecorator::process(std::vector<std::string> tokens) const {
    // Стемминг через libstemmer
    tokens = stemmer.stem(tokens);

    // Передаём дальше по цепочке
    if (wrapped) {
        return wrapped->process(std::move(tokens));
    }
    return tokens;
}