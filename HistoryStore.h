#ifndef HISTORY_STORE_H
#define HISTORY_STORE_H

#include <string>
#include <vector>
#include "HuffmanCompressor.h"

struct HistoryRow {
    std::string timestamp;
    int originalBits;
    int compressedBits;
    double percentSaved;
    double timeMs;
};

// Adds one row to the CSV (creates the file + header if needed). Returns false on failure.
bool appendHistory(const CompressionResult& result,
                   const std::string& path = "history.csv");

// Reads all rows back. Returns an empty vector if the file doesn't exist yet.
std::vector<HistoryRow> loadHistory(const std::string& path = "history.csv");

#endif