#include "InvertedIndex.h"
#include <algorithm>

void InvertedIndex::addTerm(const std::string& term, int docId, int pos) {
    auto& postings = index[term];

    auto it = std::find_if(postings.begin(), postings.end(),
        [docId](const Posting& p) { return p.documentId == docId; });

    if (it != postings.end()) {
        it->termFrequency++;
        it->positions.push_back(pos);
    } else {
        Posting newPosting;
        newPosting.documentId = docId;
        newPosting.termFrequency = 1;
        newPosting.positions.push_back(pos);
        postings.push_back(newPosting);
    }
}

std::vector<Posting> InvertedIndex::getPostings(const std::string& term) const {
    auto it = index.find(term);
    if (it != index.end()) {
        return it->second;
    }
    return {};
}

void InvertedIndex::addDocument(int docId, const Document& doc) {
    documents[docId] = doc;
}

std::optional<Document> InvertedIndex::getDocument(int docId) const {
    auto it = documents.find(docId);
    if (it != documents.end()) {
        return it->second;
    }
    return std::nullopt;
}

const std::map<int, Document>& InvertedIndex::getDocuments() const {
    return documents;
}

size_t InvertedIndex::getDocumentCount() const {
    return documents.size();
}
void InvertedIndex::setDocLength(int docId, int length) {
    docLengths[docId] = length;
}

int InvertedIndex::getDocLength(int docId) const {
    auto it = docLengths.find(docId);
    if (it != docLengths.end()) {
        return it->second;
    }
    return 0;
}

double InvertedIndex::getAvgDocLength() const {
    if (docLengths.empty()) {
        return 1.0;  
    }

    long long total = 0;
    for (const auto& [docId, len] : docLengths) {
        total += len;
    }
    return static_cast<double>(total) / docLengths.size();
}
void InvertedIndex::clear() {
    index.clear();
    documents.clear();
}