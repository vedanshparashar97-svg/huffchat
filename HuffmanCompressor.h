#ifndef HUFFMAN_COMPRESSOR_H
#define HUFFMAN_COMPRESSOR_H

#include <string>
#include <unordered_map>
#include "HuffmanNode.h"

struct CompressionResult {
    std::string encoded;                          // string of '0' and '1'
    std::unordered_map<char, std::string> codes;  // char -> code
    HuffmanNode* root = nullptr;                  // caller must deleteTree() this
    int originalBits = 0;
    int compressedBits = 0;
    double percentSaved = 0.0;
    double timeTakenMs = 0.0;
};

class HuffmanCompressor {
public:
    CompressionResult compress(const std::string& message) const;
    std::string decompress(const std::string& encoded, HuffmanNode* root) const;
};

#endif