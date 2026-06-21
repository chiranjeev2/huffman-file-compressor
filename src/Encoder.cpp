#include "Encoder.hpp"
#include "BitReaderWriter.hpp"
#include <queue>
#include <vector>
#include <string>   // Added explicitly
#include <fstream>
#include <iostream>

Encoder::~Encoder() { delete root; }

void Encoder::generateCodes(HuffmanNode* node, const std::string& str) {
    if (!node) return;
    if (node->isLeaf()) {
        huffmanCodes[node->data] = str.empty() ? "0" : str; // Handle single character edge-case
        return;
    }
    generateCodes(node->left, str + "0");
    generateCodes(node->right, str + "1");
}

bool Encoder::compress(const std::string& inputPath, const std::string& outputPath) {
    std::ifstream inFile(inputPath, std::ios::binary);
    if (!inFile.is_open()) return false;

    // 1. Compute frequencies
    freqTable.clear();
    uint64_t totalBytes = 0;
    char ch;
    while (inFile.get(ch)) {
        freqTable[static_cast<uint8_t>(ch)]++;
        totalBytes++;
    }
    
    if (totalBytes == 0) { // Edge Case: Empty File
        BitWriter writer(outputPath);
        uint32_t magic = HUFF_MAGIC;
        uint64_t size = 0;
        uint16_t tableSize = 0;
        writer.getStream().write(reinterpret_cast<char*>(&magic), 4);
        writer.getStream().write(reinterpret_cast<char*>(&size), 8);
        writer.getStream().write(reinterpret_cast<char*>(&tableSize), 2);
        return true;
    }

    // 2. Build Tree
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, NodeComparator> minHeap;
    for (auto& pair : freqTable) {
        minHeap.push(new HuffmanNode(pair.first, pair.second));
    }

    while (minHeap.size() > 1) {
        auto left = minHeap.top(); minHeap.pop();
        auto right = minHeap.top(); minHeap.pop();
        auto parent = new HuffmanNode(left->frequency + right->frequency, left, right);
        minHeap.push(parent);
    }
    root = minHeap.top();

    // 3. Map Codes
    generateCodes(root, "");

    // 4. Write to Custom Binary Format
    BitWriter writer(outputPath);
    auto& outStream = writer.getStream();

    uint32_t magic = HUFF_MAGIC;
    uint16_t tableSize = static_cast<uint16_t>(freqTable.size());

    outStream.write(reinterpret_cast<char*>(&magic), sizeof(magic));
    outStream.write(reinterpret_cast<char*>(&totalBytes), sizeof(totalBytes));
    outStream.write(reinterpret_cast<char*>(&tableSize), sizeof(tableSize));

    for (auto& pair : freqTable) {
        uint8_t b = pair.first;
        uint64_t f = pair.second;
        outStream.write(reinterpret_cast<char*>(&b), sizeof(b));
        outStream.write(reinterpret_cast<char*>(&f), sizeof(f));
    }

    inFile.clear();
    inFile.seekg(0, std::ios::beg);
    while (inFile.get(ch)) {
        writer.writeBits(huffmanCodes[static_cast<uint8_t>(ch)]);
    }

    return true;
}