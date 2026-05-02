#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>
#include "Types.h"

class InvertedIndex {
private:
    std::map<std::string, std::vector<Posting>> index;
    std::map<int, Document> documents;

public:
    // Термы
    void addTerm(const std::string& term, int docId, int pos);
    std::vector<Posting> getPostings(const std::string& term) const;

    // Документы
    void addDocument(int docId, const Document& doc);
    std::optional<Document> getDocument(int docId) const;
    const std::map<int, Document>& getDocuments() const;
    size_t getDocumentCount() const;


    void clear();
};