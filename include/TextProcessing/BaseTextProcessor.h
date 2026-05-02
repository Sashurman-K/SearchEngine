#pragma once

#include "ITextProcessor.h"

class BaseTextProcessor : public ITextProcessor {
public:
    std::vector<std::string> process(std::vector<std::string> tokens) const override;
};
