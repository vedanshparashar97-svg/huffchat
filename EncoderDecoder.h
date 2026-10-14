#ifndef ENCODER_DECODER_H
#define ENCODER_DECODER_H
#include <string>
#include <unordered_map>
#include "TreeBuilder.h" 

std::string encode(
    const std::string& message,
    const std::unordered_map<char, std::string>& codes
);

std::string decode(
    const std::string& encoded,
    HuffmanNode* root
);

#endif
