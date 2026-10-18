#include "HuffmanCompressor.h"
#include <chrono>
#include "frequency_map.h"
#include "TreeBuilder.h"
#include "EncoderDecoder.h"

CompressionResult HuffmanCompressor::compress(const std::string& message) const {
    CompressionResult result;

    auto start = std::chrono::steady_clock::now();

    auto freqMap = buildFrequencyMap(message);
    result.root = buildHuffmanTree(freqMap);
    result.codes = generateCodes(result.root);
    result.encoded = encode(message, result.codes);

    auto end = std::chrono::steady_clock::now();
    result.timeTakenMs =
        std::chrono::duration<double, std::milli>(end - start).count();

    // Stats (your part)
    result.originalBits = static_cast<int>(message.size()) * 8;
    result.compressedBits = static_cast<int>(result.encoded.size());

    if (result.originalBits > 0) {
        result.percentSaved =
            100.0 * (result.originalBits - result.compressedBits) / result.originalBits;
    }

    return result;
}

std::string HuffmanCompressor::decompress(const std::string& encoded,
                                           HuffmanNode* root) const {
    return decode(encoded, root);
}