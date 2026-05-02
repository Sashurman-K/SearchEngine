#pragma once

#include <string>
#include <vector>

class ITextProcessor {
public:
    virtual ~ITextProcessor() = default;
    virtual std::vector<std::string> process(std::vector<std::string> tokens) const = 0;
};