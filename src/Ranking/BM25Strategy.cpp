#include "BM25Strategy.h"
#include <cmath>
#include <algorithm>
#include <set>
#include <map>

void BM25Strategy::setParameters(double k1, double b) {
    this->k1 = k1;
    this->b = b;
}

std::vector<Result> BM25Strategy::rank(const QueryNode& query,
                                        const InvertedIndex& index)
{
    std::vector<Result> results;

    if (query.terms.empty() || index.getDocumentCount() == 0) {
        return results;
    }

    int totalDocs = static_cast<int>(index.getDocumentCount());
    double avgDocLength = index.getAvgDocLength();

    // Собираем все документы, где есть хотя бы один терм запроса
    std::set<int> relevantDocs;
    for (const auto& term : query.terms) {
        for (const auto& p : index.getPostings(term)) {
            relevantDocs.insert(p.documentId);
        }
    }

    // Считаем BM25 для каждого документа
    for (int docId : relevantDocs) {
        double score = 0.0;
        double docLength = static_cast<double>(index.getDocLength(docId));

        // Если длина не сохранена — используем среднюю
        if (docLength == 0.0) {
            docLength = avgDocLength;
        }

        for (const auto& term : query.terms) {
            auto postings = index.getPostings(term);

            int df = static_cast<int>(postings.size());
            if (df == 0) continue;

            // IDF
            double idf = std::log(
                (totalDocs - df + 0.5) / (df + 0.5) + 1.0
            );

            // TF для этого документа
            double tf = 0.0;
            for (const auto& p : postings) {
                if (p.documentId == docId) {
                    tf = static_cast<double>(p.termFrequency);
                    break;
                }
            }

            if (tf == 0.0) continue;

            // Формула BM25
            double numerator = tf * (k1 + 1.0);
            double denominator = tf + k1 * (1.0 - b + b * (docLength / avgDocLength));
            score += idf * (numerator / denominator);
        }

        if (score > 0.0) {
            auto doc = index.getDocument(docId);
            if (doc.has_value()) {
                results.push_back({doc.value(), score});
            }
        }
    }

    std::sort(results.begin(), results.end(),
        [](const Result& a, const Result& b) {
            return a.score > b.score;
        });

    return results;
}