#include "TreeBuilder.h"
#include <queue>
#include <vector>

struct Compare {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq; 
    }
};

HuffmanNode* buildHuffmanTree(const std::unordered_map<char, int>& freqMap) {
    if (freqMap.empty()) return nullptr;

    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, Compare> pq;
    for (const auto& pair : freqMap) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }

    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();
        HuffmanNode* merged = new HuffmanNode('\0', left->freq + right->freq, left, right);
        pq.push(merged);
    }

    return pq.top();
}

void generateCodesHelper(HuffmanNode* node, std::string code, std::unordered_map<char, std::string>& codes) {
    if (!node) return;
    if (node->isLeaf()) {
        codes[node->ch] = code;
        return;
    }
    generateCodesHelper(node->left, code + "0", codes);
    generateCodesHelper(node->right, code + "1", codes);
}

std::unordered_map<char, std::string> generateCodes(HuffmanNode* root) {
    std::unordered_map<char, std::string> codes;
    if (!root) return codes;

    if (root->isLeaf()) {
        codes[root->ch] = "0";
        return codes;
    }

    generateCodesHelper(root, "", codes);
    return codes;
}

void deleteTree(HuffmanNode* root) {
    if (!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}