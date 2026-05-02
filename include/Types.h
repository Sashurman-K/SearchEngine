#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct Document {
    int id;
    std::string title;
    std::string path;
    std::uintmax_t size;
};

struct Result {
    Document doc;
    int score;
};

struct Posting {
    int documentId;
    int termFrequency;
    std::vector<int> positions;
};

struct QueryNode {
    std::vector<std::string> terms;
};