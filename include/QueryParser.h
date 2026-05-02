#pragma once
#include "Types.h"
#include <string>

class QueryParser {
public:
    QueryNode parse(const std::string& query);
};