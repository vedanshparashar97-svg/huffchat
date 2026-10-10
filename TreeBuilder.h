#ifndef TREE_BUILDER_H
#define TREE_BUILDER_H

#include <unordered_map>
#include <string>
#include "HuffmanNode.h"

HuffmanNode* buildHuffmanTree(const std::unordered_map<char, int>& freqMap);
std::unordered_map<char, std::string> generateCodes(HuffmanNode* root);
void deleteTree(HuffmanNode* root);

#endif 