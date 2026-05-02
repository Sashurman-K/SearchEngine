#pragma once

#include "ITextProcessor.h"
#include <memory>

class TextDecorator : public ITextProcessor {
protected:
    std::unique_ptr<ITextProcessor> wrapped;

public:
    explicit TextDecorator(std::unique_ptr<ITextProcessor> processor);
    virtual ~TextDecorator() = default;

    // Делегирует обёрнутому процессору.
    // Конкретные декораторы вызывают wrapped->process() внутри себя.
    std::vector<std::string> process(std::vector<std::string> tokens) const override;
};