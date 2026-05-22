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
    std::map<int, int> docLengths;
public:
    InvertedIndex() = default;
    InvertedIndex(const InvertedIndex& other);
    InvertedIndex& operator=(const InvertedIndex& other);
    // Термы
    void addTerm(const std::string& term, int docId, int pos);
    std::vector<Posting> getPostings(const std::string& term) const;

    // Документы
    void addDocument(int docId, const Document& doc);
    std::optional<Document> getDocument(int docId) const;
    const std::map<int, Document>& getDocuments() const;
    size_t getDocumentCount() const;

    double getAvgDocLength() const;
    void setDocLength(int docId, int length);
    int getDocLength(int docId) const;

    void clear();
};