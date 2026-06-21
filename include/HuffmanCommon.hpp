#pragma once
#include <cstdint>
#include <memory>

struct HuffmanNode {
    uint8_t data;
    uint64_t frequency;
    HuffmanNode* left;
    HuffmanNode* right;

    // Leaf node
    HuffmanNode(uint8_t val, uint64_t freq) 
        : data(val), frequency(freq), left(nullptr), right(nullptr) {}

    // Internal node
    HuffmanNode(uint64_t freq, HuffmanNode* l, HuffmanNode* r) 
        : data(0), frequency(freq), left(l), right(r) {}

    ~HuffmanNode() {
        delete left;
        delete right;
    }

    bool isLeaf() const {
        return !left && !right;
    }
};

struct NodeComparator {
    bool operator()(const HuffmanNode* lhs, const HuffmanNode* rhs) const {
        return lhs->frequency > rhs->frequency;
    }
};

const uint32_t HUFF_MAGIC = 0x48554646; // "HUFF"