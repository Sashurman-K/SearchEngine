#include "TFIDFStrategy.h"
#include <cmath>
#include <algorithm>
#include <map>
#include <iostream>

double TFIDFStrategy::calculateTFIDF(const std::string& term, int docId,
                                      const InvertedIndex& index) const
{
    auto postings = index.getPostings(term);
    if (postings.empty()) {
        return 0.0;
    }

    int totalDocs = static_cast<int>(index.getDocumentCount());
    int df = static_cast<int>(postings.size());

    double tf = 0.0;
    for (const auto& p : postings) {
        if (p.documentId == docId) {
            tf = static_cast<double>(p.termFrequency);
            break;
        }
    }

    if (tf == 0.0 || df == 0 || totalDocs == 0) {
        return 0.0;
    }

    double idf = std::log(1.0 + static_cast<double>(totalDocs) / df);
    return tf * idf;
}

std::vector<Result> TFIDFStrategy::rank(const QueryNode& query,
                                         const InvertedIndex& index)
{
    std::cout << "DEBUG TFIDF::rank: terms=" << query.terms.size()
              << " totalDocs=" << index.getDocumentCount()
              << " avgLen=" << index.getAvgDocLength() << "\n";

    std::vector<Result> results;

    if (query.terms.empty() || index.getDocumentCount() == 0) {
        std::cout << "DEBUG: empty query or no docs\n";
        return results;
    }

    std::map<int, double> docScores;

    for (const auto& term : query.terms) {
        auto postings = index.getPostings(term);
        std::cout << "DEBUG: term='" << term << "' postings=" << postings.size() << "\n";
        for (const auto& p : postings) {
            double tfidf = calculateTFIDF(term, p.documentId, index);
            std::cout << "DEBUG:   docId=" << p.documentId << " tfidf=" << tfidf << "\n";
            docScores[p.documentId] += tfidf;
        }
    }

    std::cout << "DEBUG: docScores size=" << docScores.size() << "\n";

    for (const auto& [docId, score] : docScores) {
        auto doc = index.getDocument(docId);
        std::cout << "DEBUG: docId=" << docId << " score=" << score
                  << " hasDoc=" << doc.has_value() << "\n";
        if (doc.has_value() && score > 0.0) {
            results.push_back({doc.value(), score});
        }
    }

    std::cout << "DEBUG: results=" << results.size() << "\n";

    std::sort(results.begin(), results.end(),
        [](const Result& a, const Result& b) {
            return a.score > b.score;
        });

    return results;
}