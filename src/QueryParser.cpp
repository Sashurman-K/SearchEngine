#include "QueryParser.h"
#include <sstream>
#include <algorithm>
#include <cctype>

QueryNode QueryParser::parse(const std::string& query) {
    QueryNode node;
    std::istringstream stream(query);
    std::string word;

    while (stream >> word) {
        // Приводим к нижнему регистру
        std::transform(word.begin(), word.end(), word.begin(),
            [](unsigned char c) { return std::tolower(c); });

        // Убираем знаки препинания по краям
        word.erase(std::remove_if(word.begin(), word.end(),
            [](unsigned char c) { return std::ispunct(c); }),
            word.end());

        if (!word.empty()) {
            node.terms.push_back(word);
        }
    }

    return node;
}