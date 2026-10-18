#include "HistoryStore.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <ctime>

static std::mutex historyMutex;  // the server handles requests on several threads

static std::string currentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm tmNow{};
#ifdef _WIN32
    localtime_s(&tmNow, &now);
#else
    localtime_r(&now, &tmNow);
#endif
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tmNow);
    return buf;
}

bool appendHistory(const CompressionResult& r, const std::string& path) {
    std::lock_guard<std::mutex> lock(historyMutex);

    bool needHeader = false;
    {
        std::ifstream check(path);
        needHeader = !check.good() ||
                     check.peek() == std::ifstream::traits_type::eof();
    }

    std::ofstream out(path, std::ios::app);
    if (!out) return false;

    if (needHeader) {
        out << "timestamp,original_bits,compressed_bits,percent_saved,time_ms\n";
    }

    out << currentTimestamp() << ','
        << r.originalBits << ','
        << r.compressedBits << ','
        << std::fixed << std::setprecision(2) << r.percentSaved << ','
        << std::setprecision(3) << r.timeTakenMs << '\n';

    return static_cast<bool>(out);
}

std::vector<HistoryRow> loadHistory(const std::string& path) {
    std::lock_guard<std::mutex> lock(historyMutex);

    std::vector<HistoryRow> rows;
    std::ifstream in(path);
    if (!in) return rows;

    std::string line;
    std::getline(in, line);  // skip header

    while (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string ts, a, b, c, d;
        if (!std::getline(ss, ts, ',') || !std::getline(ss, a, ',') ||
            !std::getline(ss, b, ',')  || !std::getline(ss, c, ',') ||
            !std::getline(ss, d, ','))
            continue;  // skip broken lines instead of crashing

        try {
            rows.push_back({ts, std::stoi(a), std::stoi(b),
                            std::stod(c), std::stod(d)});
        } catch (...) {
            continue;
        }
    }
    return rows;
}