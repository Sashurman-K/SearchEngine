#include "TextDecorator.h"

TextDecorator::TextDecorator(std::unique_ptr<ITextProcessor> processor)
    : wrapped(std::move(processor)) {}

std::vector<std::string> TextDecorator::process(std::vector<std::string> tokens) const {
    if (wrapped) {
        return wrapped->process(std::move(tokens));
    }
    return tokens;
}