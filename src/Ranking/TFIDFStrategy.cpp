#include "TFIDFStrategy.h"
#include <cmath>
#include <algorithm>
#include <map>

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

    double idf = std::log(static_cast<double>(totalDocs) / df);
    return tf * idf;
}

std::vector<Result> TFIDFStrategy::rank(const QueryNode& query,
                                         const InvertedIndex& index)
{
    std::vector<Result> results;

    if (query.terms.empty() || index.getDocumentCount() == 0) {
        return results;
    }

    std::map<int, double> docScores;

    for (const auto& term : query.terms) {
        auto postings = index.getPostings(term);
        for (const auto& p : postings) {
            docScores[p.documentId] += calculateTFIDF(term, p.documentId, index);
        }
    }

    for (const auto& [docId, score] : docScores) {
        auto doc = index.getDocument(docId);
        if (doc.has_value() && score > 0.0) {
            results.push_back({doc.value(), score});
        }
    }

    std::sort(results.begin(), results.end(),
        [](const Result& a, const Result& b) {
            return a.score > b.score;
        });

    return results;
}