#include <iostream>
#include <iomanip>
#include "HuffmanCompressor.h"
#include "HistoryStore.h"
#include "TreeBuilder.h"
using namespace std;

int main() {
    HuffmanCompressor compressor;
    string tests[] = {
        "hello world",
        "aaaaaa",
        "",
        "this is a longer message to check that the savings improve with length"
    };

    for (const string& msg : tests) {
        CompressionResult r = compressor.compress(msg);
        string back = compressor.decompress(r.encoded, r.root);

        cout << "Message   : \"" << msg << "\"\n";
        cout << "Original  : " << r.originalBits << " bits\n";
        cout << "Compressed: " << r.compressedBits << " bits\n";
        cout << "Saved     : " << fixed << setprecision(2) << r.percentSaved << "%\n";
        cout << "Time      : " << setprecision(3) << r.timeTakenMs << " ms\n";
        cout << "Round trip: " << (back == msg ? "PASS" : "FAIL") << "\n\n";

        appendHistory(r);
        deleteTree(r.root);   // free the tree once we're done with it
    }

    cout << "--- history.csv ---\n";
    for (const auto& row : loadHistory()) {
        cout << row.timestamp << " | " << row.originalBits << " | "
             << row.compressedBits << " | " << row.percentSaved << " | "
             << row.timeMs << "\n";
    }
    return 0;
}